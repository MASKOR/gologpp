#include "Action.h"
#include "EffectAxiom.h"
#include "atoms.h"

#include <eclipseclass.h>
#include <utility>

namespace gologpp {
namespace readylog {

Action::Action(generic::Action &&action)
: Translatable<generic::Action, tuple<EC_word, EC_word, EC_word>>(std::move(action))
{}


void Action::init(tuple<EC_word, EC_word, EC_word> &cache)
{
	EC_word *args = new EC_word[arity()];
	EC_word prim_action = ::term(EC_functor("prim_action", 1), ::term(
	                  EC_functor(name().c_str(), arity()), args));

	EC_word poss = dynamic_cast<const Translatable<generic::BooleanExpression, EC_word> &>(precondition()).translate();
	EC_word causes_val = dynamic_cast<const gologpp::readylog::EffectAxiom &>(effect()).translate();

	cache = std::tie(prim_action, poss, causes_val);
}


}
}