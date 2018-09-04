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

string AbstractAction::to_string(const string &) const
{ return name() + '(' + concat_list(args(), ", ", "") + ")"; }



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


void Action::define(
	boost::optional<BooleanExpression *> precondition,
	boost::optional<vector<AbstractEffectAxiom *>> effects
) {
	set_precondition(precondition.get_value_or(Constant<BooleanExpression>(true)));
	if (effects)
		effects_ = vector<unique_ptr<AbstractEffectAxiom>> {
			effects.value().begin(), effects.value().end()
		};
}


string Action::to_string(const string &pfx) const
{
	return linesep + pfx + "action " + AbstractAction::to_string(pfx) + " {"
		+ linesep + pfx + "precondition:" + linesep + precondition().to_string(pfx + indent)
		+ linesep + pfx + "effect:" + linesep + concat_list(effects(), ";" linesep, pfx) + linesep "}";
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
{}

string AbstractTransition::to_string(const string &) const
{ return action().name() + '(' + concat_list(args(), ", ") + ')'; }



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
