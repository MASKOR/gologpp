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

#include "platform_backend.h"
#include "execution.h"
#include "activity.h"

namespace gologpp {

PlatformBackend *Clock::clock_source = nullptr;

Clock::time_point Clock::now() noexcept
{ return clock_source->time(); }



PlatformBackend::~PlatformBackend()
{}

shared_ptr<Activity> PlatformBackend::start_activity(const Transition &trans)
{
	Lock l(lock());
	shared_ptr<Activity> a = std::make_shared<Activity>(trans, *exec_ctx_);
	auto it = activities_.find(a->hash());
	if (it != activities_.end())
		throw UserError(
			"Cannot start an action while another one with the same arguments is already running."
			" Currently: " + it->second->str()
		);
	a->attach_semantics(exec_context()->semantics_factory());
	execute_activity(a);
	a->set_state(Activity::target_state(trans.hook()));
	activities_.insert({a->hash(), a});
	return a;
}

void PlatformBackend::cancel_activity(const Transition &trans)
{
	Lock l(lock());
	auto it = activities_.find(trans.hash());
	if (it == activities_.end())
		throw Bug("Activity lost: " + trans->str());
	else
		preempt_activity(std::dynamic_pointer_cast<Activity>(it->second));
}


PlatformBackend::Lock PlatformBackend::lock()
{ return Lock(mutex_); }


shared_ptr<Activity> PlatformBackend::end_activity(const Transition &trans)
{
	Lock l(lock());

	ActivityMap::iterator it = activities_.find(trans.hash());
	shared_ptr<Activity> dur_running;
	if (it != activities_.end())
		dur_running = std::dynamic_pointer_cast<Activity>(it->second);

	if (!dur_running)
		throw LostTransition(trans->str());

	// Either the durative action's state exactly matches the primitive action's hook,
	// or the hook is END, which may be executed if the durative state is either FINAL, FAILED or CANCELLED
	if ((trans.hook() == Transition::Hook::END
			&& (
				dur_running->state() == Activity::State::FINAL
				|| dur_running->state() == Activity::State::FAILED
				|| dur_running->state() == Activity::State::CANCELLED
			)
		)
		|| dur_running->state() == Activity::target_state(trans.hook())
	) {
		activities_.erase(it);
		return dur_running;
	}
	else
		throw InconsistentTransition(trans->str());
}


void PlatformBackend::set_context(AExecutionContext *ctx)
{
	exec_ctx_ = ctx;
	ready_condition_.notify_all();
}


Activity::State PlatformBackend::current_state(const Grounding<Action> &a)
{
	Lock l(lock());

	auto it = activities_.find(a.hash());
	if (it == activities_.end())
		return Activity::State::IDLE;
	else {
		return it->second->cast<Activity>().state();
	}
}


AExecutionContext *PlatformBackend::exec_context()
{ return exec_ctx_.load(); }

void PlatformBackend::wait_until_ready()
{
	std::unique_lock<std::mutex> ready_lock(ready_mutex_);
	ready_condition_.wait(
		ready_lock,
		[&] () -> bool { return exec_context(); }
	);
}



}
