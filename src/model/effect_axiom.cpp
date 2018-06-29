#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom(const shared_ptr<AbstractAction> &action, unique_ptr<BooleanExpression> &&condition)
: action_(action)
, condition_(std::move(condition))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const shared_ptr<AbstractAction> AbstractEffectAxiom::action() const
{ return action_.lock(); }


const BooleanExpression &AbstractEffectAxiom::condition() const
{ return *condition_; }


} // namespace gologpp
