/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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
: AbstractSemantics<InitialValue>(v)
{}

EC_word Semantics<InitialValue>::plterm()
{
	EC_word fluent_inst;
	if (element().fluent().arity() > 0)
		fluent_inst = ::term(
			EC_functor(element().fluent().name().c_str(), element().fluent().arity()),
			to_ec_words(element().args()).data()
		);
	else
		fluent_inst = EC_atom(element().fluent().name().c_str());

	return ::term(EC_functor("initial_val", 2),
		fluent_inst,
		element().value().semantics().plterm()
	);
}



EC_word Semantics<Fluent>::plterm()
{
	if (element().arity() > 0)
		return ::term(
			EC_functor(element().name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().name().c_str());
}


vector<EC_word> Semantics<Fluent>::initially()
{
	vector<EC_word> rv;
	for (const unique_ptr<InitialValue> &ival : element().initially())
		rv.push_back(ival->semantics().plterm());

	return rv;
}


EC_word Semantics<Fluent>::prim_fluent()
{
	element().scope().semantics().init_vars();

	vector<EC_word> arg_domains;
	for (const shared_ptr<Variable> &arg : element().params())
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
