#include "fluent.h"
#include "execution.h"
#include "scope.h"
#include "utilities.h"

#include <model/fluent.h>
#include <model/platform_backend.h>

#include <iostream>


extern "C"
int p_exog_fluent_getValue()
{
	using namespace gologpp;
	EC_word fluent = EC_arg(1);
	EC_word value = EC_arg(2);
	EC_atom fluent_atom;
	EC_functor fluent_fn;

	if (fluent.is_atom(&fluent_atom) == EC_succeed) {
		string name(fluent_atom.name());
		if (name == "now")
			return value.unify(EC_word(
				ReadylogContext::instance().context_time()
					.time_since_epoch().count()
			));
		else
			throw EclipseError(string("Unhandled exog_prim_fluent ") + name);
	}
	else if (fluent.functor(&fluent_fn) == EC_succeed) {
		throw EclipseError(string("Unhandled exog_prim_fluent ")
			+ fluent_fn.name() + "/" + std::to_string(fluent_fn.arity())
		);
	}
	else {
		throw EclipseError("Invalid exog_prim_fluent");
	}
}



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
