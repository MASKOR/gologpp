#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom()
: condition_(new BooleanConstant(true))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const AbstractAction &AbstractEffectAxiom::action() const
{ return *action_; }

AbstractAction &AbstractEffectAxiom::action()
{ return *action_; }

void AbstractEffectAxiom::set_action(AbstractAction &action)
{ action_ = &action; }

const BooleanExpression &AbstractEffectAxiom::condition() const
{ return *condition_; }

BooleanExpression &AbstractEffectAxiom::condition()
{ return *condition_; }

void AbstractEffectAxiom::set_condition(BooleanExpression *condition)
{
	condition_.reset(condition);
	condition_->set_parent(this);
}

} // namespace gologpp
