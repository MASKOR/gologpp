#include "action.h"
#include "effect_axiom.h"
#include "execution.h"
#include "error.h"
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
{
	set_precondition(new BooleanConstant(true));
	vector<Expression *> mapping_args;
	for (const shared_ptr<AbstractVariable> &arg : args) {
		switch(arg->dynamic_type_tag()) {
		case BOOLEAN:
			mapping_args.push_back(new Reference<BooleanVariable>(std::dynamic_pointer_cast<BooleanVariable>(arg)));
			break;
		case NUMERIC:
			mapping_args.push_back(new Reference<NumericVariable>(std::dynamic_pointer_cast<NumericVariable>(arg)));
			break;
		case SYMBOLIC:
			mapping_args.push_back(new Reference<SymbolicVariable>(std::dynamic_pointer_cast<SymbolicVariable>(arg)));
			break;
		case VOID:
			throw Bug("Variable<Statement> is impossible");
		}
	}
	mapping_.reset(new ActionMapping(name, mapping_args));
}

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

const ActionMapping& Action::mapping() const
{
	return *mapping_;
}

void Action::set_mapping(ActionMapping *mapping)
{
	mapping_.reset(mapping);
}

bool Action::blocking() const
{
	return !(bool(on_succeed_) || bool(on_preempted_) || bool(on_failed_));
}

unique_ptr<Reference<Procedure>> &Action::on_succeed()
{
	return on_succeed_;
}

unique_ptr<Reference<Procedure>> &Action::on_preempted()
{
	return on_preempted_;
}

unique_ptr<Reference<Procedure>> &Action::on_failed()
{
	return on_failed_;
}

void Action::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		scope().attach_semantics(implementor);
		precondition_->attach_semantics(implementor);
		for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
			effect->attach_semantics(implementor);
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


void ExogAction::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		scope().attach_semantics(implementor);
		for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
			effect->attach_semantics(implementor);
	}
}


string ExogAction::to_string(const string &pfx) const
{
	return linesep + pfx + "event" + AbstractAction::to_string(pfx) + " {"
		+ linesep + pfx + "effect:" + linesep + concat_list(effects(), ";" linesep, pfx) + linesep "}";
}



AbstractTransition::AbstractTransition(const AbstractTransition &other)
: action_(other.action_)
{
	for (const unique_ptr<AbstractConstant> &c : other.args()) {
		AbstractConstant *cc = c->copy();
		dynamic_cast<Expression *>(cc)->set_parent(this);
		args_.emplace_back(cc);
	}
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



Transition::Transition(const Transition &other)
: AbstractTransition(other)
{}

void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->attach_semantics(implementor);
	}
}



ExogTransition::ExogTransition(const ExogTransition &other)
: AbstractTransition(other)
{}

void ExogTransition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<AbstractConstant> &c : args_)
			c->attach_semantics(implementor);
	}
}



} // namespace gologpp
