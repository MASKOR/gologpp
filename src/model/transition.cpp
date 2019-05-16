#include "transition.h"
#include "constant.h"
#include "action.h"

namespace gologpp {



Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<Constant>> &&args, Hook hook)
: Grounding<Action>(action, std::move(args))
, hook_(hook)
{}

Transition::Hook Transition::hook() const
{ return hook_; }

void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<Constant> &c : args())
			c->attach_semantics(implementor);
	}
}

string Transition::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(hook()) + "(" + Grounding<Action>::to_string(pfx) + ")"; }

string to_string(Transition::Hook h)
{
	switch (h) {
	case Transition::Hook::START:
		return "start";
	case Transition::Hook::STOP:
		return "stop";
	case Transition::Hook::FINISH:
		return "finish";
	case Transition::Hook::FAIL:
		return "fail";
	}
	throw Bug(string("Unhandled ") + typeid(h).name());
}



Activity::Activity(const shared_ptr<Action> &action, vector<unique_ptr<Constant>> &&args, State state)
: Grounding<Action>(action, std::move(args))
, state_(state)
{}

Activity::Activity(const shared_ptr<Transition> &trans)
: Grounding<Action>(trans->target(), copy(trans->args()))
, state_(State::IDLE)
{
	if (trans->hook() != Transition::Hook::START)
		throw Bug("Activity must be constructed from a START Transition");
}

void Activity::set_state(Activity::State state)
{ state_ = state; }

Activity::State Activity::state() const
{ return state_; }

shared_ptr<Transition> Activity::transition(Transition::Hook hook)
{ return std::make_shared<Transition>(target(), copy(args()), hook); }

string Activity::to_string(const string &pfx) const
{ return pfx + "state(" + Grounding<Action>::to_string("") + ") = " + gologpp::to_string(state()); }

void Activity::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<Constant> &c : args())
			c->attach_semantics(implementor);
	}
}

void Activity::set_sensing_result(Constant *sr)
{ sensing_result_.reset(sr); }

unique_ptr<Constant> &Activity::sensing_result()
{ return sensing_result_; }

const unique_ptr<Constant> &Activity::sensing_result() const
{ return sensing_result_; }


string to_string(Activity::State s)
{
	switch(s) {
	case Activity::State::IDLE:
		return "idle";
	case Activity::State::RUNNING:
		return "running";
	case Activity::State::PREEMPTED:
		return "preempted";
	case Activity::State::FINAL:
		return "final";
	case Activity::State::FAILED:
		return "failed";
	}
	throw Bug(string("Unhandled ") + typeid(s).name());
}







} // namespace gologpp
