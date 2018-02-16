#include "EffectAxiom.h"
#include "Reference.h"
#include <gologpp/EffectAxiom.h>
#include "Action.h"
#include <gologpp/Action.h>
#include "Fluent.h"
#include <gologpp/Fluent.h>
#include "Formula.h"
#include "Scope.h"


namespace gologpp {


Implementation<generic::EffectAxiom>::Implementation(const generic::EffectAxiom &eff)
: effect_(eff)
{}


EC_word Implementation<generic::EffectAxiom>::term()
{
	effect_.action().parent_scope().impl_cast<generic::Scope>().init_vars();
	return ::term(
		EC_functor("causes_val", 3),
		effect_.action().impl_cast<generic::Reference<generic::Action>>().term(),
		effect_.fluent().impl_cast<generic::Reference<generic::Fluent>>().term(),
		effect_.value().impl_cast<generic::Reference<generic::Expression>>().term()
	);
}


} // namespace gologpp
