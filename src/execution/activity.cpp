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



Activity::Activity(const shared_ptr<Action> &action, vector<unique_ptr<Expression>> &&args, AExecutionController &ctx, State state)
: ReferenceBase<Action>(action, std::move(args))
, state_(state)
, exec_context_(ctx)
{}

Activity::Activity(const Transition &trans, AExecutionController &ctx)
: ReferenceBase<Action>(trans.target(), copy(trans.args()))
, state_(State::IDLE)
, exec_context_(ctx)
{
	if (trans.hook() != Transition::Hook::START)
		throw Bug("Activity must be constructed from a START Transition");
}

const Action &Activity::operator *() const
{ return *this->target(); }

Action &Activity::operator *()
{ return *this->target(); }

const Action *Activity::operator ->() const
{ return this->target().get(); }

Action *Activity::operator ->()
{ return this->target().get(); }

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
	exec_context_.exog_queue_push(shared_from_this());
}


Value Activity::mapped_arg_value(const string &name) const
{
	return
		dynamic_cast<GeneralSemantics<Expression> &>(
			target()->mapping().mapped_expr(name).general_semantics()
		).evaluate({ &this->binding() }, exec_context_.history());
}


const std::string &Activity::mapped_name() const
{ return target()->mapping().backend_name(); }

string Activity::to_string(const string &pfx) const
{ return pfx + "state(" + ReferenceBase<Action>::to_string("") + ") = " + gologpp::to_string(state()); }


void Activity::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		binding().attach_semantics(implementor);
		semantics_ = implementor.make_semantics(*this);
		for (auto &c : args())
			c->attach_semantics(implementor);
	}
}


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
