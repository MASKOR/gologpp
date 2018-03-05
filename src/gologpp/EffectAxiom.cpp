#include "EffectAxiom.h"
#include "Action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom(Reference<Action> &&action)
: action_(std::move(action))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const Reference<Action> &AbstractEffectAxiom::action() const
{ return action_; }


} // namespace gologpp
