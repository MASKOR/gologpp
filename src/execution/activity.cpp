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
#include "context.h"


namespace gologpp {



Activity::Activity(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, AExecutionContext &ctx, State state)
: Grounding<Action>(action, std::move(args))
, state_(state)
, exec_context_(ctx)
{}

Activity::Activity(const Transition &trans, AExecutionContext &ctx)
: Grounding<Action>(trans.target(), copy(trans.args()))
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


void Activity::update(Transition::Hook hook, boost::optional<Value> &&sensing_result)
{
	PlatformBackend::Lock backend_lock = exec_context_.backend().lock();

	if (hook == Transition::Hook::FINISH) {
		if (target()->senses() && !sensing_result)
			throw Bug("PlatformBackend implementation tried to finish the sensing action "
				+ static_cast<const Grounding<Action> &>(*this).str()
				+ " without providing a sensing result"
			);
		else if (!target()->senses() && sensing_result)
			throw Bug("PlatformBackend implementation gave a sensing result to the action "
				+ static_cast<const Grounding<Action> &>(*this).str()
				+ ", but it is not a sensing action"
			);
		else if (sensing_result) {
			sensing_result->attach_semantics(exec_context_.semantics_factory());
			set_sensing_result(std::forward<boost::optional<Value>>(sensing_result));
		}
	}

	set_state(target_state(hook));

	exec_context_.exog_queue_push(shared_from_this());
}


Value Activity::mapped_arg_value(const string &name) const
{
	return
		dynamic_cast<GeneralSemantics<Expression> &>(
			target()->mapping().mapped_expr(name).general_semantics()
		).evaluate(this->binding(), exec_context_.history());
}


const std::string &Activity::mapped_name() const
{ return target()->mapping().backend_name(); }

string Activity::to_string(const string &pfx) const
{ return pfx + "state(" + Grounding<Action>::to_string("") + ") = " + gologpp::to_string(state()); }

void Activity::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		binding().attach_semantics(implementor);
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<Value> &c : args())
			c->attach_semantics(implementor);
	}
}

void Activity::set_sensing_result(boost::optional<Value> &&sr)
{ sensing_result_ = std::forward<boost::optional<Value>>(sr); }

boost::optional<Value> &Activity::sensing_result()
{ return sensing_result_; }

const boost::optional<Value> &Activity::sensing_result() const
{ return sensing_result_; }


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
