#include "Fluent.h"
#include "Reference.h"
#include "atoms.h"

namespace gologpp {
namespace readylog {


void Fluent::init(EC_word &cache)
{
	EC_functor functor(name().c_str(), arity());

	cache = ::term(EC_functor("prim_fluent", 1),
	                             ::term(functor, new EC_word[arity()]));
}


void Initially::init(EC_word &cache)
{
	cache = ::term(EC_functor("initially", 2),
	                           dynamic_cast<const Reference<Fluent>::translatable &>(fluent()).translate(),
	                           dynamic_cast<const AnyValue::translatable &>(initial_value()).translate()
	               );
}

} // namespace readylog
} // namespace gologpp
