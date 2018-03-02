#include "EffectAxiom.h"
#include "Reference.h"
#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include "Scope.h"


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
