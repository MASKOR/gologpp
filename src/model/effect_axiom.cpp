#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom(const AbstractAction *action, BooleanExpression *condition)
: action_(action)
, condition_(condition)
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const AbstractAction &AbstractEffectAxiom::action() const
{ return *action_; }


const BooleanExpression &AbstractEffectAxiom::condition() const
{ return *condition_; }


} // namespace gologpp
