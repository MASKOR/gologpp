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

#include "effect_axiom.h"
#include "procedural.h"
#include "action.h"



namespace gologpp {


string cv_functor(const AbstractEffectAxiom &eff)
{
	if (eff.action().is_a<Action>())
		// All golog++ actions are implicitly durative
		return "durative_causes_val";
	else if (eff.action().is_a<ExogAction>())
		return "causes_val";
	else
		throw Bug("Unknown action type");
}



template<>
EC_word Semantics<EffectAxiom<Reference<Fluent>>>::plterm()
{
	EC_word condition_term = element().condition().semantics().plterm();
	if (element().lhs().special_semantics().args_need_eval()) {

		condition_term = ::term(EC_functor("and", 2),
			element().lhs().special_semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor(element()).c_str(), 4),
		element().action().semantics<AbstractAction>().plterm(),
		element().lhs().semantics<Reference<Fluent>>().plterm_free_args(),
		element().value().semantics().plterm(),
		condition_term
	);
}



template<>
EC_word Semantics<EffectAxiom<FieldAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(&element().lhs(), nullptr);

	EC_word condition_term = element().condition().semantics().plterm();
	if (fluent_access.first->special_semantics().args_need_eval()) {

		condition_term = ::term(EC_functor("and", 2),
			fluent_access.first->special_semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor(element()).c_str(), 4),
		element().action().semantics<AbstractAction>().plterm(),
		fluent_access.first->special_semantics().plterm_free_args(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			element().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		condition_term
	);
}



template<>
EC_word Semantics<EffectAxiom<ListAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(nullptr, &element().lhs());

	EC_word condition_term = element().condition().semantics().plterm();
	if (fluent_access.first->special_semantics().args_need_eval()) {

		condition_term = ::term(EC_functor("and", 2),
			fluent_access.first->special_semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor(element()).c_str(), 4),
		element().action().semantics<AbstractAction>().plterm(),
		fluent_access.first->special_semantics().plterm_free_args(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			element().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		condition_term
	);
}




}
