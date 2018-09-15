#include "action.h"
#include "effect_axiom.h"
#include "execution.h"
#include <string>

namespace gologpp {


AbstractAction::AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
{}


const vector<unique_ptr<AbstractEffectAxiom>> &AbstractAction::effects() const
{ return effects_; }

void AbstractAction::add_effect(AbstractEffectAxiom *effect)
{
	effect->set_action(*this);
	effects_.emplace_back(effect);
}

void AbstractAction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

string AbstractAction::to_string(const string &) const
{ return name() + '(' + concat_list(args(), ", ", "") + ")"; }



Action::Action(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: AbstractAction(own_scope, name, args)
{ set_precondition(new BooleanConstant(true)); }

Action::Action(Scope &parent_scope, const string &name)
: Action(new Scope(parent_scope), name, {})
{}

const BooleanExpression &Action::precondition() const
{ return *precondition_; }

void Action::set_precondition(BooleanExpression *cond)
{
	precondition_.reset(cond);
	precondition_->set_parent(this);
}


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


void Action::define(
	boost::optional<BooleanExpression *> precondition,
	boost::optional<vector<AbstractEffectAxiom *>> effects
) {
	if (precondition)
		set_precondition(precondition.value());
	if (effects)
		for (AbstractEffectAxiom *e : *effects)
			add_effect(e);
}


string Action::to_string(const string &pfx) const
{
	return linesep + pfx + "action " + AbstractAction::to_string(pfx) + " {"
		+ linesep + pfx + "precondition:" + linesep + precondition().to_string(pfx + indent)
		+ linesep + pfx + "effect:" + linesep + concat_list(effects(), ";" linesep, pfx) + linesep "}";
}


Reference<Action> *Action::make_ref(const vector<Expression *> &args)
{ return make_ref_<Action>(args); }

Expression *Action::ref(const vector<Expression *> &args)
{ return make_ref(args); }


void ExogAction::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope().implement(implementor);
		for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
			effect->implement(implementor);
	}
}


string ExogAction::to_string(const string &pfx) const
{
	return linesep + pfx + "event" + AbstractAction::to_string(pfx) + " {"
		+ linesep + pfx + "effect:" + linesep + concat_list(effects(), ";" linesep, pfx) + linesep "}";
}


const Action &AbstractTransition::action() const
{ return *action_; }

const vector<unique_ptr<AbstractConstant>> &AbstractTransition::args() const
{ return args_; }



AbstractTransition::AbstractTransition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args)
: action_(action)
, args_(std::move(args))
{
	for (unique_ptr<AbstractConstant> &c : args_)
		dynamic_cast<Expression *>(c.get())->set_parent(this);
}

string AbstractTransition::to_string(const string &) const
{ return action().name() + '(' + concat_list(args(), ", ") + ')'; }

Scope &AbstractTransition::parent_scope()
{ return global_scope(); }

const Scope &AbstractTransition::parent_scope() const
{ return global_scope(); }



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
