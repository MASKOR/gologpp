#include "Action.h"

#include <eclipseclass.h>
#include <utility>

namespace gologpp {
namespace readylog {

Action::Action(gologpp::Action &&action)
: gologpp::Translatable<gologpp::Action, tuple<EC_word, EC_word, EC_word>>(std::move(action))
{
	EC_word *args = new EC_word[arity()];

	EC_word prim_action = ::term(EC_functor("prim_action", 1), ::term(
	                  EC_functor(name().c_str(), arity()), args));
	EC_word precond = dynamic_cast<const Expression &>(precondition())
}

tuple<EC_word, EC_word, EC_word> Action::translate()
{ return cache_; }


}
}