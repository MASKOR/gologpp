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
#include <model/platform/component_backend.h>

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


shared_ptr<AbstractEvent> AExecutionController::exog_queue_pop()
{
	std::lock_guard<std::mutex> locked{ exog_mutex_ };
	shared_ptr<AbstractEvent> rv = std::move(exog_queue_.front());
	exog_queue_.pop_front();
	rv->attach_semantics(semantics_factory());
	return rv;
}


shared_ptr<AbstractEvent> AExecutionController::exog_queue_poll(optional<Clock::time_point> timeout)
{
	exog_queue_block(timeout);
	return exog_queue_pop();
}


void AExecutionController::exog_queue_block(optional<Clock::time_point> timeout)
{
	std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
	{
		std::lock_guard<std::mutex> l1{ wait_mutex_ };
		auto condition = [&] {
			return !exog_queue_.empty() || terminated;
		};
		if (timeout)
			queue_empty_condition_.wait_until(queue_empty_lock, timeout.value(), condition);
		else
			queue_empty_condition_.wait(queue_empty_lock, condition);
	}

	if (terminated)
		throw Terminate();
	else if (exog_queue_.empty())
		throw ExogTimeout();
}


void AExecutionController::terminate()
{
	std::lock_guard<std::mutex> l1 { exog_mutex_ };
	platform_backend_->terminate();
	terminated = true;
	queue_empty_condition_.notify_all();

	// Don't return until the execution thread has unblocked from the exog_queue
	std::lock_guard<std::mutex> l2 { wait_mutex_ };
}


void AExecutionController::exog_queue_push(shared_ptr<AbstractEvent> exog)
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	exog_queue_.push_back(std::move(exog));
	{
		std::lock_guard<std::mutex> { queue_empty_mutex_ };
		queue_empty_condition_.notify_one();
	}
}

void AExecutionController::exog_queue_push_front(shared_ptr<AbstractEvent> exog)
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
	exog_queue_push_front(shared_ptr<ExogEvent> { new ExogEvent {
		step_time_action_,
		{ unique_ptr<Value>( new Value (
			get_type<NumberType>(),
			Clock::now().time_since_epoch().count()
		) ) }
	} } );

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
		shared_ptr<AbstractEvent> exog = exog_queue_pop();

		if (!std::dynamic_pointer_cast<Reference<platform::SwitchStateAction>>(exog)) {
			// Nothing to do here for SwitchStateAction:
			// its effects have already been applied to the component model
			if (!exog->ref()->silent()) {
				log(LogLevel::INF) << ">>> Exogenous event: " << exog->ref() << flush;
				silent_ = false;
			}

			shared_ptr<Activity> activity = std::dynamic_pointer_cast<Activity>(exog);
			if (activity && activity->action()->senses())
				history().general_semantics<History>().append_sensing_result(
					activity,
					activity->action()->senses()->lhs(),
					activity->action()->senses()->rhs().general_semantics().evaluate(
						{ &activity->ref().binding() },
						history()
					)
				);

			history().general_semantics<History>().append(exog);
		}
	}
}

void AExecutionController::drain_exog_queue_blocking(optional<Clock::time_point> timeout)
{
	if (!silent_)
		log(LogLevel::DBG) << "=== No transition possible: Waiting for exogenous events..." << flush;

	exog_queue_block(timeout);
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



}
