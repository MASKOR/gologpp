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
#include <model/procedural.h>
#include <model/effect_axiom.h>
#include <model/logger.h>
#include <model/reference.h>

#include <model/platform/semantics.h>
#include <model/platform/switch_state_action.h>

#include "plan.h"
#include "context.h"
#include "history.h"
#include "platform_backend.h"
#include "dummy_backend.h"
#include "activity.h"


#include <iostream>
#include <iomanip>



namespace gologpp {


AExecutionContext::AExecutionContext(unique_ptr<PlatformBackend> &&platform_backend)
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
	Scope *action_scope = new Scope(global_scope());

	/***************************************************************************
	 * Define platform/temporal execution model                                */
	global_scope().register_global(new platform::SwitchStateAction(action_scope));

	Fluent *f_time = global_scope().define_global<Fluent, const vector<InitialValue *>>(
		new Scope(global_scope()),
		get_type<NumberType>(),
		"context_time",
		boost::none,
		{ new InitialValue(boost::none, new Value(get_type<NumberType>(), Clock::now().time_since_epoch().count())) }
	);

	Scope *ea_scope = new Scope(global_scope());
	shared_ptr<Variable> time_var = ea_scope->get_var(VarDefinitionMode::FORCE, get_type<NumberType>(), "next_time");
	step_time_action_.reset(global_scope().define_global<ExogAction>(
		ea_scope,
		get_type<VoidType>(),
		"step_context_time",
		boost::optional<vector<shared_ptr<Variable>>>{{ time_var }},
		boost::none,
		boost::none,
		boost::none
	));

	EffectAxiom<Reference<Fluent>> *time_eff = new EffectAxiom<Reference<Fluent>>();
	time_eff->define(boost::none, f_time->make_ref({}), new Reference<Variable>(time_var));
	time_eff->set_action(*step_time_action_);
	step_time_action_->add_effect(time_eff);
	/***************************************************************************/
}


shared_ptr<Reference<AbstractAction>> AExecutionContext::exog_queue_pop()
{
	std::lock_guard<std::mutex> locked{ exog_mutex_ };
	shared_ptr<Reference<AbstractAction>> rv = std::move(exog_queue_.front());
	exog_queue_.pop();
	return rv;
}


shared_ptr<Reference<AbstractAction>> AExecutionContext::exog_queue_poll()
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
	else
		return exog_queue_pop();
}


void AExecutionContext::terminate()
{
	std::lock_guard<std::mutex> l1 { exog_mutex_ };
	platform_backend_->terminate();
	terminated = true;
	queue_empty_condition_.notify_all();
	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


void AExecutionContext::exog_queue_push(shared_ptr<Reference<AbstractAction>> exog)
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	exog_queue_.push(std::move(exog));
	{
		std::lock_guard<std::mutex> { queue_empty_mutex_ };
		queue_empty_condition_.notify_one();
	}
}


void AExecutionContext::exog_timer_wakeup()
{
	exog_queue_push(shared_ptr<Reference<AbstractAction>> {
		step_time_action_->make_ref(
			{ new Value(
				get_type<NumberType>(),
				Clock::now().time_since_epoch().count())
			}
		)
	} );

	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


bool AExecutionContext::exog_empty()
{
	std::lock_guard<std::mutex> l(exog_mutex_);
	return exog_queue_.empty();
}

SemanticsFactory &AExecutionContext::semantics_factory() const
{ return *semantics_; }

PlatformBackend &AExecutionContext::backend()
{ return *platform_backend_;}

History &AExecutionContext::history()
{ return history_; }

void AExecutionContext::drain_exog_queue()
{
	while (!exog_empty()) {
		shared_ptr<Reference<AbstractAction>> r = exog_queue_pop();
		Reference<ExogAction> &exog = r->cast<Reference<ExogAction>>();
		if (!exog->silent()) {
			log(LogLevel::INF) << ">>> Exogenous event: " << exog << flush;
			silent_ = false;
		}
		exog->attach_semantics(semantics_factory());
		history().general_semantics<History>().append(exog);
	}
}

void AExecutionContext::drain_exog_queue_blocking()
{
	if (!silent_)
		log(LogLevel::INF) << "=== No transition possible: Waiting for exogenous events..." << flush;

	shared_ptr<Reference<AbstractAction>> exog = exog_queue_poll();
	if (exog) {
		if (!(*exog)->silent()) {
			log(LogLevel::INF) << ">>> Exogenous event: " << exog << flush;
			silent_ = false;
		}
		exog->attach_semantics(semantics_factory());
		history().general_semantics<History>().append(exog);
		drain_exog_queue();
	}
	else
		log(LogLevel::DBG)
			<< ">>> Timer wakeup: "
			<< context_time()
			<< flush;
}

bool AExecutionContext::silent() const
{ return silent_; }

void AExecutionContext::set_silent(bool silent)
{ silent_ = silent; }

Clock::time_point AExecutionContext::context_time() const
{
	unique_ptr<Expression> context_time {
		global_scope().lookup_global<Fluent>("context_time")->make_ref({})
	};
	context_time->attach_semantics(semantics_factory());

	Binding empty_binding;
	empty_binding.attach_semantics(semantics_factory());

	*context_time_ = Clock::time_point(Clock::duration(
		context_time->general_semantics().evaluate(
			empty_binding,
			history_
		).numeric_convert<Clock::rep>()
	));

	return *context_time_;
}

shared_ptr<platform::SwitchStateAction> AExecutionContext::switch_state_action()
{ return switch_state_action_; }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ExecutionContext::ExecutionContext(
	unique_ptr<PlatformBackend> &&exec_backend,
	unique_ptr<PlanTransformation> &&plan_transformation
)
: AExecutionContext(std::move(exec_backend))
, plan_transformation_(std::move(plan_transformation))
{
	if (!plan_transformation_)
		plan_transformation_.reset(semantics_factory().platform_semantics_factory().make_transformation());
}

ExecutionContext::~ExecutionContext()
{}


void ExecutionContext::run(Block &&program)
{
	try {
		history().attach_semantics(semantics_factory());
		global_scope().implement_globals(semantics_factory(), *this);

		program.attach_semantics(semantics_factory());
		compile(program);

		plan_transformation_->init(*this);

		Binding empty_binding;
		empty_binding.attach_semantics(semantics_factory());

		while (!program.general_semantics().final(empty_binding, history())) {
			set_silent(true);

			unique_ptr<Plan> plan {
				program.general_semantics().trans(empty_binding, history())
			};

			if (plan) {
				plan = plan_transformation_->transform(std::move(*plan));

				log(LogLevel::DBG) << "Got plan: " << *plan << flush;

				while (!plan->elements().empty()) {
					if (terminated)
						throw Terminate();

					if (!exog_empty()) {
						drain_exog_queue();
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
							.general_semantics().trans(empty_binding, history())
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
						if (context_time() > plan->elements().front().latest_timepoint()) {
							// First plan element's time window has passed: replan!
							plan = plan_transformation_->transform(std::move(*plan));
							log(LogLevel::DBG) << "Replanned: " << *plan << flush;
						}
					}

					if (history().general_semantics<History>().should_progress()) {
						std::cout << "=== Progressing history." << std::endl;
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
		std::cout << ">>> Terminated." << std::endl;
	}
}




}
