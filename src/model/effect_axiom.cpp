#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom(shared_ptr<Action> &action, unique_ptr<BooleanExpression> &&condition)
: action_(action)
, condition_(std::move(condition))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const Action &AbstractEffectAxiom::action() const
{ return *action_; }


const BooleanExpression &AbstractEffectAxiom::condition() const
{ return *condition_; }


} // namespace gologpp
