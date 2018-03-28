#include "Action.h"
#include "EffectAxiom.h"
#include <string>

namespace gologpp {


AbstractAction::AbstractAction(const string &name, const vector<string> &args)
: Statement(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, scope_(this, {}, Scope::global_scope())
, args_(args)
{}


const vector<unique_ptr<AbstractEffectAxiom>> &AbstractAction::effects() const
{ return effects_; }


const vector<string> &AbstractAction::arg_names() const
{ return args_; }


shared_ptr<AbstractVariable> AbstractAction::argument(arity_t idx) const
{ return scope_.variable(args_[static_cast<size_t>(idx)]); }


Scope &AbstractAction::scope()
{ return scope_; }

const Scope &AbstractAction::scope() const
{ return scope_; }


const BooleanExpression &Action::precondition() const
{ return *precondition_; }


void Action::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	scope_.implement(implementor);
	precondition_->implement(implementor);
	for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
		effect->implement(implementor);
}


void ExogAction::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	scope_.implement(implementor);
	for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
		effect->implement(implementor);
}


const Action &AbstractTransition::action() const
{ return *action_; }


const vector<unique_ptr<AbstractConstant>> &AbstractTransition::args() const
{ return args_; }


void Transition::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	for (unique_ptr<AbstractConstant> &c : args_)
		c->implement(implementor);
}


void ExogTransition::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	for (unique_ptr<AbstractConstant> &c : args_)
		c->implement(implementor);
}


} // namespace gologpp
