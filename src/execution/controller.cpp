/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include <model/fluent.h>
#include <model/action.h>
#include <model/mapping.h>
#include <model/procedural.h>
#include <model/effect_axiom.h>
#include <model/logger.h>
#include <model/reference.h>

#include <model/platform/semantics.h>
#include <model/platform/switch_state_action.h>

#include "plan.h"
#include "controller.h"
#include "history.h"
#include "platform_backend.h"
#include "dummy_backend.h"
#include "activity.h"



namespace gologpp {


AExecutionController::AExecutionController(unique_ptr<PlatformBackend> &&platform_backend)
: platform_backend_(move(platform_backend))
, semantics_(&SemanticsFactory::get())
, silent_(false)
, context_time_(new Clock::time_point())
, terminated(false)
{
	if (!platform_backend_)
		platform_backend_ = std::make_unique<DummyBackend>();
	platform_backend_->set_context(this);
	Clock::init();
	semantics_->set_context(*this);

	/***************************************************************************
	 * Define platform/temporal execution model                                */
	Scope *action_scope = new Scope(global_scope());

	global_scope().register_global(new platform::SwitchStateAction(action_scope));
	switch_state_action_ = global_scope().lookup_global<platform::SwitchStateAction>("switch_state");

	Fluent *f_time = global_scope().define_global<Fluent, const vector<InitialValue *>>(
		new Scope(global_scope()),
		get_type<NumberType>(),
		"context_time",
		boost::none,
		{ new InitialValue(boost::none, new Value(get_type<NumberType>(), Clock::now().time_since_epoch().count())) }
	);

	Scope *ea_scope = new Scope(global_scope());
	shared_ptr<Variable> time_var = ea_scope->get_var(VarDefinitionMode::FORCE, get_type<NumberType>(), "next_time");
	global_scope().define_global<ExogAction>(
		ea_scope,
		get_type<VoidType>(),
		"step_context_time",
		boost::optional<vector<shared_ptr<Variable>>>{{ time_var }},
		boost::none,
		boost::none,
		boost::none
	);
	step_time_action_ = global_scope().lookup_global<ExogAction>("step_context_time");

	EffectAxiom<Reference<Fluent>> *time_eff = new EffectAxiom<Reference<Fluent>>();
	time_eff->define(boost::none, f_time->make_ref({}), new Reference<Variable>(time_var));
	time_eff->set_action(*step_time_action_);
	step_time_action_->add_effect(time_eff);
	/***************************************************************************/
}


shared_ptr<Reference<AbstractAction>> AExecutionController::exog_queue_pop()
{
	std::lock_guard<std::mutex> locked{ exog_mutex_ };
	shared_ptr<Reference<AbstractAction>> rv = std::move(exog_queue_.front());
	exog_queue_.pop_front();
	return rv;
}


shared_ptr<Reference<AbstractAction>> AExecutionController::exog_queue_poll()
{
	exog_queue_block();
	return exog_queue_pop();
}


void AExecutionController::exog_queue_block()
{
	std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
	{
		std::lock_guard<std::mutex> l1{ wait_mutex_ };
		queue_empty_condition_.wait(queue_empty_lock, [&] {
			return !exog_queue_.empty() || terminated;
		});
	}

	if (terminated)
		throw Terminate();
}


void AExecutionController::terminate()
{
	std::lock_guard<std::mutex> l1 { exog_mutex_ };
	platform_backend_->terminate();
	terminated = true;
	queue_empty_condition_.notify_all();
	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


void AExecutionController::exog_queue_push(shared_ptr<Reference<AbstractAction>> exog)
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	exog_queue_.push_back(std::move(exog));
	{
		std::lock_guard<std::mutex> { queue_empty_mutex_ };
		queue_empty_condition_.notify_one();
	}
}

void AExecutionController::exog_queue_push_front(shared_ptr<Reference<AbstractAction> > exog)
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	exog_queue_.push_front(std::move(exog));
	{
		std::lock_guard<std::mutex> { queue_empty_mutex_ };
		queue_empty_condition_.notify_one();
	}
}


void AExecutionController::exog_timer_wakeup()
{
	exog_queue_push_front(shared_ptr<Reference<AbstractAction>> {
		step_time_action_->make_ref(
			{ new Value(
				get_type<NumberType>(),
				Clock::now().time_since_epoch().count())
			}
		)
	} );

	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


bool AExecutionController::exog_empty()
{
	std::lock_guard<std::mutex> l(exog_mutex_);
	return exog_queue_.empty();
}

SemanticsFactory &AExecutionController::semantics_factory() const
{ return *semantics_; }

PlatformBackend &AExecutionController::backend()
{ return *platform_backend_;}

History &AExecutionController::history()
{ return history_; }

void AExecutionController::drain_exog_queue()
{
	while (!exog_empty()) {
		shared_ptr<Reference<AbstractAction>> exog = exog_queue_pop();

		if (!std::dynamic_pointer_cast<Reference<platform::SwitchStateAction>>(exog)) {
			// Nothing to do here for SwitchStateAction:
			// its effects have already been applied to the component model
			if (!(*exog)->silent()) {
				log(LogLevel::INF) << ">>> Exogenous event: " << exog << flush;
				silent_ = false;
			}
			exog->attach_semantics(semantics_factory());

			shared_ptr<Activity> activity = std::dynamic_pointer_cast<Activity>(exog);
			if (activity && activity->target()->senses())
				history().general_semantics<History>().append_sensing_result(
					activity,
					activity->target()->senses()->lhs(),
					activity->target()->senses()->rhs().general_semantics().evaluate(
						{ &activity->binding() },
						history()
					)
				);

			history().general_semantics<History>().append(exog);
		}
	}
}

void AExecutionController::drain_exog_queue_blocking()
{
	if (!silent_)
		log(LogLevel::DBG) << "=== No transition possible: Waiting for exogenous events..." << flush;

	exog_queue_block();
	drain_exog_queue();
}

bool AExecutionController::silent() const
{ return silent_; }

void AExecutionController::set_silent(bool silent)
{ silent_ = silent; }

Clock::time_point AExecutionController::context_time() const
{
	unique_ptr<Expression> context_time {
		global_scope().lookup_global<Fluent>("context_time")->make_ref({})
	};
	context_time->attach_semantics(semantics_factory());

	*context_time_ = Clock::time_point(Clock::duration(
		context_time->general_semantics().evaluate(
			{},
			history_
		).numeric_convert<Clock::rep>()
	));

	return *context_time_;
}

shared_ptr<platform::SwitchStateAction> AExecutionController::switch_state_action()
{ return switch_state_action_; }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ExecutionController::ExecutionController(
	unique_ptr<PlatformBackend> &&exec_backend,
	unique_ptr<PlanTransformation> &&plan_transformation
)
: AExecutionController(std::move(exec_backend))
, plan_transformation_(std::move(plan_transformation))
{
	if (!plan_transformation_)
		plan_transformation_.reset(semantics_factory().platform_semantics_factory().make_transformation());
}

ExecutionController::~ExecutionController()
{}


void ExecutionController::run(const Instruction &program)
{
	try {
		history().attach_semantics(semantics_factory());
		global_scope().implement_globals(semantics_factory(), *this);

		plan_transformation_->init(*this);

		while (!program.general_semantics().final({}, history())) {
			set_silent(true);

			unique_ptr<Plan> plan {
				program.general_semantics().trans({}, history())
			};

			if (plan) {
				plan = plan_transformation_->transform(std::move(*plan));

				log(LogLevel::INF) << "<<< Transformed schedule: " << *plan << flush;

				while (!plan->elements().empty()) {
					set_silent(true);

					if (terminated)
						throw Terminate();

					if (!exog_empty()) {
						drain_exog_queue();
						if (backend().any_component_state_changed_exog())
							plan = plan_transformation_->transform(std::move(*plan));
					}

					unique_ptr<Plan> empty_plan;

					if (plan->elements().front().earliest_timepoint() > context_time()) {
						backend().schedule_timer_event(
							plan->elements().front().earliest_timepoint()
						);
					}
					else {
						// Plan elements are expected to not return plans again (nullptr or empty Plan).
						empty_plan = plan->elements().front().instruction()
							.general_semantics().trans({}, history())
						;
					}

					if (empty_plan) {
						// Empty plan: successfully executed
						if (!empty_plan->elements().empty())
							throw Bug(
								"Plan instruction returned a plan: "
								+ plan->elements().front().instruction().str()
							);
						plan->elements().erase(plan->elements().begin());
					}
					else {
						// Current Plan element not executable
						drain_exog_queue_blocking();

						if (context_time() > plan->elements().front().latest_timepoint()
							|| backend().any_component_state_changed_exog()
						) {
							// First plan element's time window has passed: replan!
							log(LogLevel::INF) << "=== Re-transforming..." << flush;
							plan = plan_transformation_->transform(std::move(*plan));
							log(LogLevel::INF) << "=== New schedule " << *plan << flush;
						}
					}

					if (history().general_semantics<History>().should_progress()) {
						log(LogLevel::DBG) << "=== Progressing history." << flush;
						history().general_semantics<History>().progress();
					}
				}
			}
			else {
				drain_exog_queue_blocking();
			}

			if (terminated)
				throw Terminate();

		}
	} catch (Terminate &) {
		log(LogLevel::DBG) << ">>> Terminated." << flush;
	}
}




}
