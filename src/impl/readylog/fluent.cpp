#include "fluent.h"
#include <model/fluent.h>
#include "scope.h"

#include <iostream>

namespace gologpp {

Implementation<AbstractFluent>::Implementation(const AbstractFluent &f)
: fluent_(f)
{}


EC_word Implementation<AbstractFluent>::term()
{
	return ::term(
		EC_functor(fluent_.name().c_str(), fluent_.arity()),
		fluent_.scope().implementation().variables(fluent_.args())
	);
}


EC_word Implementation<AbstractFluent>::prim_fluent()
{
	fluent_.scope().implementation().init_vars();
	return ::term(
		EC_functor("prim_fluent", 1),
		term()
	);
}

} // namespace gologpp
