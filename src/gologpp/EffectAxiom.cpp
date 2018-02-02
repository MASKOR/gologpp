#include "EffectAxiom.h"
#include "Action.h"

namespace gologpp {
namespace generic {

EffectAxiom::EffectAxiom(Reference<Action> &&action, Reference<Fluent> &&fluent, const shared_ptr<Expression> &value)
: action_(std::move(action))
, fluent_(std::move(fluent))
, value_(value)
{}

EffectAxiom::EffectAxiom(EffectAxiom &&e)
: action_(std::move(e.action_))
, fluent_(std::move(e.fluent_))
, value_(std::move(e.value_))
{}

EffectAxiom::~EffectAxiom() {
}


tuple<Reference<Action> &, Reference<Fluent> &, Expression &> EffectAxiom::members()
{ return std::tie(action_, fluent_, *value_); }


} // namespace generic
} // namespace gologpp
