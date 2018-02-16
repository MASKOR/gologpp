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

const Reference<Action> &EffectAxiom::action() const
{ return action_; }

const Reference<Fluent> &EffectAxiom::fluent() const
{ return fluent_; }

const Expression &EffectAxiom::value() const
{ return *value_; }


} // namespace generic
} // namespace gologpp
