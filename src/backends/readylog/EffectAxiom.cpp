#include "EffectAxiom.h"


namespace gologpp {
namespace readylog {


/*EffectAxiom::EffectAxiom(generic::EffectAxiom &&other)
: gologpp::Translatable<generic::EffectAxiom, EC_word>(std::forward<generic::EffectAxiom>(other))
{}


void EffectAxiom::init(EC_word &cache)
{
	EC_word fluent_ref = dynamic_cast<const readylog::Reference<readylog::Fluent> &>(fluent()).translate();
	EC_word action_ref = dynamic_cast<const readylog::Reference<readylog::Action> &>(action()).translate();
	EC_word value_expr = dynamic_cast<const Translatable<generic::Expression, EC_word> &>(value()).translate();

	cache = ::term(EC_functor("causes_val", 3), action_ref, fluent_ref, value_expr);
}*/


} // namespace readylog
} // namespace gologpp
