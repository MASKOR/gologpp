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

#include <model/platform/semantics.h>
#include <model/platform/switch_state_action.h>

#include "plan.h"
#include "context.h"
#include "history.h"
#include "platform_backend.h"
#include "dummy_backend.h"
#include "activity.h"
#include "grounding.h"


#include <iostream>



namespace gologpp {


AExecutionContext::AExecutionContext(unique_ptr<SemanticsFactory> &&semantics, unique_ptr<PlatformBackend> &&platform_backend)
: platform_backend_(move(platform_backend))
, semantics_(std::move(semantics))
, silent_(false)
, terminated(false)
, context_time_(Clock::time_point::min())
{
	Scope *action_scope = new Scope(global_scope());
	global_scope().register_global(new platform::SwitchStateAction(action_scope));

	action_scope = new Scope(global_scope());
	global_scope().define_global<ExogAction>(
		action_scope,
		get_type<VoidType>(),
		"exog_state_change",
		vector<shared_ptr<Variable>> {
			action_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "component"),
			action_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "from_state"),
			action_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "to_state"),
		},
		boost::none, /* precondition */
		boost::none, /* effects */
		boost::none  /* mapping */
	);

	if (!platform_backend_)
		platform_backend_ = std::make_unique<DummyBackend>();
	platform_backend_->set_context(this);
	Clock::clock_source = platform_backend_.get();
	context_time_ = Clock::now();

	semantics_->set_context(*this);
}


shared_ptr<Grounding<AbstractAction>> AExecutionContext::exog_queue_pop()
{
	std::lock_guard<std::mutex> locked{ exog_mutex_ };
	shared_ptr<Grounding<AbstractAction>> rv = std::move(exog_queue_.front());
	exog_queue_.pop();
	return rv;
}


shared_ptr<Grounding<AbstractAction>> AExecutionContext::exog_queue_poll()
{
	Clock::time_point prev_time = context_time();
	std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
	{
		std::lock_guard<std::mutex> l1{ wait_mutex_ };
		queue_empty_condition_.wait(queue_empty_lock, [&] {
			return !exog_queue_.empty() || terminated || context_time() > prev_time;
		});
	}

	if (terminated)
		throw Terminate();
	else if (context_time() > prev_time)
		return nullptr;
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


void AExecutionContext::exog_queue_push(shared_ptr<Grounding<AbstractAction>> exog)
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
	context_time_ = backend().time();
	queue_empty_condition_.notify_all();
	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


bool AExecutionContext::exog_empty()
{
	std::lock_guard<std::mutex> l(exog_mutex_);
	return exog_queue_.empty();
}

SemanticsFactory &AExecutionContext::semantics_factory()
{ return *semantics_; }

PlatformBackend &AExecutionContext::backend()
{ return *platform_backend_;}

History &AExecutionContext::history()
{ return history_; }

void AExecutionContext::drain_exog_queue()
{
	while (!exog_empty()) {
		shared_ptr<Grounding<AbstractAction>> exog = exog_queue_pop();
		if (!(*exog)->silent()) {
			std::cout << ">>> Exogenous event: " << exog << std::endl;
			silent_ = false;
		}
		exog->attach_semantics(semantics_factory());
		history().general_semantics<History>().append(exog);
	}
}

void AExecutionContext::drain_exog_queue_blocking()
{
	if (!silent_)
		std::cout << "=== No transition possible: Waiting for exogenous events..." << std::endl;

	shared_ptr<Grounding<AbstractAction>> exog = exog_queue_poll();
	if (exog) {
		if (!(*exog)->silent()) {
			std::cout << ">>> Exogenous event: " << exog << std::endl;
			silent_ = false;
		}
		exog->attach_semantics(semantics_factory());
		history().general_semantics<History>().append(exog);
		drain_exog_queue();
	}
}

bool AExecutionContext::silent() const
{ return silent_; }

void AExecutionContext::set_silent(bool silent)
{ silent_ = silent; }

Clock::time_point AExecutionContext::context_time() const
{ return context_time_; }



ExecutionContext::ExecutionContext(
	unique_ptr<SemanticsFactory> &&semantics,
	unique_ptr<PlatformBackend> &&exec_backend,
	unique_ptr<PlanTransformation> &&plan_transformation
)
: AExecutionContext(std::move(semantics), std::move(exec_backend))
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

		Binding<Value> empty_binding;
		empty_binding.attach_semantics(semantics_factory());

		while (!program.general_semantics().final(empty_binding, history())) {
			set_silent(true);

			unique_ptr<Plan> plan {
				program.general_semantics().trans(empty_binding, history())
			};

			if (plan) {
				plan = plan_transformation_->transform(std::move(*plan));

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
						plan = plan_transformation_->transform(std::move(*plan));
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
