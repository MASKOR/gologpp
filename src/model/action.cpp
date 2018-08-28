#include "action.h"
#include "effect_axiom.h"
#include "execution.h"
#include <string>

namespace gologpp {


AbstractAction::AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Statement(Scope::global_scope())
, Global(name, args)
, ScopeOwner(own_scope)
{}


const vector<unique_ptr<AbstractEffectAxiom>> &AbstractAction::effects() const
{ return effects_; }

void AbstractAction::add_effect(AbstractEffectAxiom *effect)
{ effects_.emplace_back(effect); }

void AbstractAction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

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



AbstractTransition::AbstractTransition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args)
: action_(action)
, args_(std::move(args))
{}



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
