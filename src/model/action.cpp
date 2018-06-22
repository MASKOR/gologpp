#include "action.h"
#include "effect_axiom.h"
#include <string>

namespace gologpp {


AbstractAction::AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Statement(Scope::global_scope())
, Global(name, args)
, scope_(own_scope)
{
	scope_->set_owner(this);
}


const vector<unique_ptr<AbstractEffectAxiom>> &AbstractAction::effects() const
{ return effects_; }

void AbstractAction::add_effect(AbstractEffectAxiom *effect)
{ effects_.emplace_back(effect); }

Scope &AbstractAction::scope()
{ return *scope_; }

const Scope &AbstractAction::scope() const
{ return *scope_; }


const BooleanExpression &Action::precondition() const
{ return *precondition_; }

void Action::set_precondition(BooleanExpression *cond)
{ precondition_.reset(cond); }


void Action::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope().implement(implementor);
		precondition_->implement(implementor);
		for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
			effect->implement(implementor);
	}
}


void ExogAction::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope().implement(implementor);
		for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
			effect->implement(implementor);
	}
}


const Action &AbstractTransition::action() const
{ return *action_; }

const vector<unique_ptr<AbstractConstant>> &AbstractTransition::args() const
{ return args_; }


void Transition::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->implement(implementor);
	}
}


void ExogTransition::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->implement(implementor);
	}
}


} // namespace gologpp
