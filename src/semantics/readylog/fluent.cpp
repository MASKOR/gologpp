#include "fluent.h"
#include "execution.h"
#include "scope.h"
#include "utilities.h"
#include "value.h"
#include "semantics.h"
#include "variable.h"

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


Semantics<InitialValue>::Semantics(const InitialValue &v)
: ival_(v)
{}

EC_word Semantics<InitialValue>::plterm()
{
	EC_word fluent_inst;
	if (ival_.fluent().arity() > 0)
		fluent_inst = ::term(
		EC_functor(ival_.fluent().name().c_str(), ival_.fluent().arity()),
		to_ec_words(ival_.args()).data()
		);
	else
		fluent_inst = EC_atom(ival_.fluent().name().c_str());

	return ::term(EC_functor("initial_val", 2),
	fluent_inst,
	ival_.value().semantics().plterm()
	);
}



Semantics<Fluent>::Semantics(const Fluent &f)
: fluent_(f)
{}

EC_word Semantics<Fluent>::plterm()
{
	if (fluent_.arity() > 0)
		return ::term(
			EC_functor(fluent_.name().c_str(), fluent_.arity()),
			to_ec_words(fluent_.params()).data()
		);
	else
		return EC_atom(fluent_.name().c_str());
}


vector<EC_word> Semantics<Fluent>::initially()
{
	vector<EC_word> rv;
	for (const unique_ptr<InitialValue> &ival : fluent_.initially())
		rv.push_back(ival->semantics().plterm());

	return rv;
}


EC_word Semantics<Fluent>::prim_fluent()
{
	fluent_.scope().semantics().init_vars();

	vector<EC_word> arg_domains;
	for (const shared_ptr<Variable> &arg : fluent_.params())
		if (arg->domain().is_defined())
			arg_domains.emplace_back(
				arg->semantics().member_restriction()
			);

	EC_word prim_fluent = ::term(EC_functor("prim_fluent", 1), plterm());

	if (arg_domains.size() > 0)
		return ::term(EC_functor(":-", 2),
			prim_fluent,
			to_ec_term(",", arg_domains)
		);
	else
		return prim_fluent;
}



} // namespace gologpp
