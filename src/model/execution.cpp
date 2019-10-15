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

#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"
#include "history.h"
#include "platform_backend.h"
#include "activity.h"

#include <iostream>


namespace gologpp {


AExecutionContext::AExecutionContext(unique_ptr<SemanticsFactory> &&semantics, unique_ptr<PlatformBackend> &&platform_backend)
: platform_backend_(move(platform_backend))
, semantics_(std::move(semantics))
, terminated(false)
{
	if (!platform_backend_)
		platform_backend_ = std::make_unique<DummyBackend>();
	platform_backend_->set_context(this);
	Clock::clock_source = platform_backend_.get();
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
	std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
	{
		std::lock_guard<std::mutex> l1{ wait_mutex_ };
		queue_empty_condition_.wait(queue_empty_lock, [&] { return !exog_queue_.empty() || terminated; });
	}

	if (terminated)
		return nullptr;
	else
		return exog_queue_pop();
}


void AExecutionContext::terminate()
{
	std::lock_guard<std::mutex> l1 { exog_mutex_ };
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



ExecutionContext::ExecutionContext(unique_ptr<SemanticsFactory> &&semantics, unique_ptr<PlatformBackend> &&exec_backend)
: AExecutionContext(std::move(semantics), std::move(exec_backend))
{}

ExecutionContext::~ExecutionContext()
{}

Clock::time_point ExecutionContext::context_time() const
{ return context_time_; }

void ExecutionContext::run(Block &&program)
{
	history().attach_semantics(semantics_factory());
	global_scope().implement_globals(semantics_factory(), *this);

	program.attach_semantics(semantics_factory());
	compile(program);

	while (!final(program, history()) && !terminated) {
		context_time_ = backend().time();
		while (!exog_empty()) {
			shared_ptr<Grounding<AbstractAction>> exog = exog_queue_pop();
			std::cout << ">>> Exogenous event: " << exog << std::endl;
			exog->attach_semantics(semantics_factory());
			history().abstract_semantics().append_exog(exog);
		}

		if (trans(program, history())) {
			shared_ptr<Transition> trans = history().abstract_semantics().get_last_transition();
			if (trans) {
				std::cout << "<<< trans: " << trans->str() << std::endl;
				if (trans->hook() == Transition::Hook::STOP)
					backend().preempt_activity(trans);
				else if (trans->hook() == Transition::Hook::START)
					backend().start_activity(trans);
				else if (trans->hook() == Transition::Hook::FINISH && trans->target()->senses())
					history().abstract_semantics().append_sensing_result(backend().end_activity(trans));
				else
					backend().end_activity(trans);
			}
		}
		else {
			std::cout << "=== No transition possible: Waiting for exogenous events..." << std::endl;
			shared_ptr<Grounding<AbstractAction>> exog = exog_queue_poll();
			if (exog) {
				std::cout << ">>> Exogenous event: " << exog << std::endl;
				exog->attach_semantics(semantics_factory());
				history().abstract_semantics().append_exog(exog);
			}
		}
	}

	if (terminated)
		std::cout << ">>> Terminated." << std::endl;
}




}
