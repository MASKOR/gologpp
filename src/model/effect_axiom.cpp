#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom(Reference<Action> &&action, unique_ptr<BooleanExpression> &&condition)
: action_(std::move(action))
, condition_(std::move(condition))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const Reference<Action> &AbstractEffectAxiom::action() const
{ return action_; }


const BooleanExpression &AbstractEffectAxiom::condition() const
{ return *condition_; }


} // namespace gologpp
