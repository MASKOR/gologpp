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

#include "value.h"
#include "action.h"
#include "effect_axiom.h"
#include "scope.h"
#include "variable.h"

#include <eclipseclass.h>


namespace gologpp {



EC_word Semantics<Action>::durative_action()
{
	element().scope().semantics().init_vars();

	vector<EC_word> arg_domains;
	for (const shared_ptr<Variable> &param : element().params())
		if (param->type().is<Domain>())
			arg_domains.emplace_back(
				param->special_semantics().member_restriction()
			);

	EC_word prim_action = ::term(EC_functor("durative_action", 1), plterm());

	if (arg_domains.size() > 0)
		return ::term(EC_functor(":-", 2),
			prim_action,
			to_ec_term(",", arg_domains)
		);
	else
		return prim_action;
}


EC_word Semantics<Action>::plterm()
{
	if (element().arity() > 0)
		return ::term(
			EC_functor(element().mangled_name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().mangled_name().c_str());
}


vector<EC_word> Semantics<Action>::durative_causes_vals()
{
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : element().effects())
		rv.push_back(effect->semantics<AbstractEffectAxiom>().plterm());
	return rv;
}


EC_word Semantics<Action>::durative_poss()
{
	element().scope().semantics().init_vars();
	return ::term(EC_functor("durative_poss", 2),
		plterm(),
		element().precondition().semantics().plterm()
	);
}


EC_word Semantics<Action>::senses()
{
	element().scope().semantics().init_vars();
	return ::term(EC_functor("senses", 2),
		plterm(),
		element().senses()->semantics().plterm()
	);
}



EC_word Semantics<ExogAction>::exog_action()
{
	element().scope().semantics().init_vars();
	return ::term(EC_functor("exog_action", 1), plterm());
}


EC_word Semantics<ExogAction>::plterm()
{
	if (element().arity() > 0)
		return ::term(
			EC_functor(element().mangled_name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().mangled_name().c_str());
}


vector<EC_word> Semantics<ExogAction>::causes_vals()
{
	element().scope().semantics().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : element().effects())
		rv.push_back(effect->semantics<AbstractEffectAxiom>().plterm());
	return rv;
}


EC_word Semantics<ExogAction>::poss()
{
	element().scope().semantics().init_vars();
	return ::term(EC_functor("poss", 2),
		plterm(),
		element().precondition().semantics().plterm()
	);
}



template<>
EC_word Semantics<Reference<ExogAction>>::plterm()
{ return reference_term(element()); }




} // namespace gologpp
