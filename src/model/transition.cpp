#include "transition.h"
#include "constant.h"
#include "action.h"

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <iostream>

namespace gologpp {



AbstractTransition::AbstractTransition(const AbstractTransition &other)
: action_(other.action_)
{
	for (const unique_ptr<AbstractConstant> &c : other.args()) {
		AbstractConstant *cc = c->copy();
		dynamic_cast<Expression *>(cc)->set_parent(this);
		args_.emplace_back(cc);
	}
}


AbstractTransition::AbstractTransition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args)
: action_(action)
, args_(std::move(args))
{
	for (unique_ptr<AbstractConstant> &c : args_)
		dynamic_cast<Expression *>(c.get())->set_parent(this);
}


AbstractTransition &AbstractTransition::operator = (const AbstractTransition &other)
{
	action_ = other.action_;
	for (const unique_ptr<AbstractConstant> &c : other.args()) {
		AbstractConstant *cc = c->copy();
		dynamic_cast<Expression *>(cc)->set_parent(this);
		args_.emplace_back(cc);
	}
	return *this;
}


bool AbstractTransition::operator == (const AbstractTransition &other) const
{
	if (action() != other.action())
		return false;
	for (arity_t i = 0; i < action()->arity(); ++i) {
		if (*args()[i] != *other.args()[i])
			return false;
	}
	return true;
}

bool AbstractTransition::operator != (const AbstractTransition &other) const
{ return !(*this == other); }

shared_ptr<Action> AbstractTransition::action() const
{ return action_; }

const vector<unique_ptr<AbstractConstant>> &AbstractTransition::args() const
{ return args_; }

string AbstractTransition::to_string(const string &) const
{ return action()->name() + '(' + concat_list(args(), ", ") + ')'; }

Scope &AbstractTransition::parent_scope()
{ return global_scope(); }

const Scope &AbstractTransition::parent_scope() const
{ return global_scope(); }




Activity::Activity(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, State state)
: AbstractTransition(action, std::move(args))
, state_(state)
{}


Activity::Activity(const shared_ptr<Transition> &trans)
: AbstractTransition(trans->action(), copy(trans->args()))
, state_(State::IDLE)
{
	if (trans->hook() != Transition::Hook::START)
		throw Bug("Activity must be constructed from a START Transition");
}


Activity &Activity::operator = (const Activity &other)
{
	AbstractTransition::operator = (other);
	state_ = other.state();
	return *this;
}

void Activity::set_state(Activity::State state)
{ state_ = state; }

Activity::State Activity::state() const
{ return state_; }

shared_ptr<Transition> Activity::transition(Transition::Hook hook) const
{ return std::make_shared<Transition>(action_, copy(args_), hook); }

void Activity::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->attach_semantics(implementor);
	}
}

string Activity::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(state()) + "(" + AbstractTransition::to_string(pfx) + ")"; }



Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, Hook hook)
: AbstractTransition(action, std::move(args))
, hook_(hook)
{}

Transition::Hook Transition::hook() const
{ return hook_; }


size_t AbstractTransition::hash() const
{
	size_t h_args = action()->hash();
	for (const unique_ptr<AbstractConstant> &c : args())
		boost::hash_combine(h_args, c->hash());

	return h_args;
}


Transition &Transition::operator = (const Transition &t)
{
	AbstractTransition::operator = (t);
	hook_ = t.hook();
	return *this;
}


void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->attach_semantics(implementor);
	}
}

string Transition::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(hook()) + "(" + AbstractTransition::to_string(pfx) + ")"; }



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
