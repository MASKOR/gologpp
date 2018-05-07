#include "effect_axiom.h"
#include "reference.h"
#include <model/action.h>
#include <model/fluent.h>
#include "scope.h"


namespace gologpp {


Implementation<EffectAxiom>::Implementation(const EffectAxiom &eff)
: effect_(eff)
{}


EC_word Implementation<EffectAxiom>::term()
{
	effect_.action().parent_scope().impl_cast<Scope>().init_vars();
	return ::term(
		EC_functor("causes_val", 3),
		effect_.action().impl_cast<Reference<Action>>().term(),
		effect_.fluent().impl_cast<Reference<Fluent>>().term(),
		dynamic_cast<ReadylogImplementation &>(effect_.value().implementation()).term()
	);
}


} // namespace gologpp
