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

#include "activity.h"
#include "controller.h"

#include <model/mapping.h>


namespace gologpp {



Activity::Activity(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, AExecutionController &ctx, State state)
: action_ref_(action, std::move(args))
, state_(state)
, exec_context_(ctx)
{}


Activity::Activity(const Transition &trans, AExecutionController &ctx)
: action_ref_(trans.action(), copy(trans.ref().args()))
, state_(State::IDLE)
, exec_context_(ctx)
{
	if (trans.hook() != Transition::Hook::START)
		throw Bug("Activity must be constructed from a START Transition");
}

void Activity::set_state(Activity::State state)
{ state_ = state; }

Activity::State Activity::state() const
{ return state_; }


Activity::State Activity::target_state(Transition::Hook hook)
{
	switch (hook) {
	case Transition::Hook::START:
		return Activity::State::RUNNING;
	case Transition::Hook::CANCEL:
		return Activity::State::CANCELLED;
	case Transition::Hook::FINISH:
		return Activity::State::FINAL;
	case Transition::Hook::FAIL:
		return Activity::State::FAILED;
	case Transition::Hook::END:
		return Activity::State::IDLE;
	}
	throw Bug("Unhandled Transition hook. Ignored warnings when compiling?");
}


void Activity::update(Transition::Hook hook)
{
	PlatformBackend::Lock backend_lock = exec_context_.backend().lock();
	set_state(target_state(hook));
	exec_context_.exog_queue_push(
		shared_ptr<Transition>(new Transition(action(), args(), hook))
	);
	update_condition_.notify_all();
}

void Activity::wait_for_update()
{
	std::unique_lock<std::mutex> update_lock { update_mutex_ };
	State prev_state = state();
	update_condition_.wait(update_lock, [&] {
		return state() != prev_state;
	});
}

Value Activity::mapped_arg_value(const string &name) const
{
	return
		dynamic_cast<GeneralSemantics<Expression> &>(
			action()->mapping().mapped_expr(name).general_semantics()
		).evaluate({ &this->ref().binding() }, exec_context_.history());
}

const std::string &Activity::mapped_name() const
{ return action()->mapping().backend_name(); }

shared_ptr<Action> Activity::action() const
{ return ref().target(); }


vector<unique_ptr<Value>> Activity::args() const
{
	vector<unique_ptr<Value>> rv;
	for (const Value *arg : ref().args())
		rv.emplace_back(new Value(*arg));
	return rv;
}

const Reference<Action, Value> &Activity::ref() const
{ return action_ref_; }

Reference<Action, Value> &Activity::ref()
{ return action_ref_; }


string to_string(Activity::State s)
{
	switch(s) {
	case Activity::State::IDLE:
		return "idle";
	case Activity::State::RUNNING:
		return "running";
	case Activity::State::CANCELLED:
		return "preempted";
	case Activity::State::FINAL:
		return "final";
	case Activity::State::FAILED:
		return "failed";
	}
	throw Bug(string("Unhandled ") + typeid(s).name());
}



} // namespace gologpp
