#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom()
: condition_(new Constant(BoolType::name(), true))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const AbstractAction &AbstractEffectAxiom::action() const
{ return *action_; }

AbstractAction &AbstractEffectAxiom::action()
{ return *action_; }

void AbstractEffectAxiom::set_action(AbstractAction &action)
{ action_ = &action; }

const Expression &AbstractEffectAxiom::condition() const
{ return *condition_; }

Expression &AbstractEffectAxiom::condition()
{ return *condition_; }

void AbstractEffectAxiom::set_condition(Expression *condition)
{
	condition_ = condition;
	condition_->set_parent(this);
}

} // namespace gologpp
