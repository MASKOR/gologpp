#include "fluent.h"
#include <model/fluent.h>
#include "scope.h"
#include "utilities.h"

#include <iostream>

namespace gologpp {

Semantics<AbstractFluent>::Semantics(const AbstractFluent &f)
: fluent_(f)
{}


EC_word Semantics<AbstractFluent>::plterm()
{
	if (fluent_.arity() > 0)
		return ::term(
			EC_functor(fluent_.name().c_str(), fluent_.arity()),
			to_ec_words(fluent_.args()).data()
		);
	else
		return EC_atom(fluent_.name().c_str());
}




} // namespace gologpp
