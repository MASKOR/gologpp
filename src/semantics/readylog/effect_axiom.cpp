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



namespace gologpp {


Semantics<AbstractEffectAxiom>::Semantics(const AbstractEffectAxiom &eff)
: AbstractSemantics<AbstractEffectAxiom>(eff)
{
	if (element().action().is_a<Action>())
		// All golog++ actions are implicitly durative
		cv_functor = "durative_causes_val";
	else if (element().action().is_a<ExogAction>())
		cv_functor = "causes_val";
	else
		throw Bug("Unknown action type");
}



const EffectAxiom<Reference<Fluent>> &Semantics<EffectAxiom<Reference<Fluent>>>::effect() const
{ return dynamic_cast<const EffectAxiom<Reference<Fluent>> &>(element()); }


EC_word Semantics<EffectAxiom<Reference<Fluent>>>::plterm()
{
	effect().scope().semantics().init_vars();

	EC_word condition_term = effect().condition().semantics().plterm();
	if (effect().lhs().semantics().args_need_eval()) {

		effect().lhs().target()->scope().semantics().init_vars();

		condition_term = ::term(EC_functor("and", 2),
			effect().lhs().semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		effect().lhs().semantics().plterm_free_args(),
		effect().value().semantics().plterm(),
		condition_term
	);
}



const EffectAxiom<FieldAccess> &Semantics<EffectAxiom<FieldAccess>>::effect() const
{ return dynamic_cast<const EffectAxiom<FieldAccess> &>(element()); }

EC_word Semantics<EffectAxiom<FieldAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(&effect().lhs(), nullptr);

	effect().scope().semantics().init_vars();

	EC_word condition_term = element().condition().semantics().plterm();
	if (fluent_access.first->semantics().args_need_eval()) {

		fluent_access.first->target()->scope().semantics().init_vars();

		condition_term = ::term(EC_functor("and", 2),
			fluent_access.first->semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		fluent_access.first->semantics().plterm_free_args(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			effect().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		condition_term
	);
}



const EffectAxiom<ListAccess> &Semantics<EffectAxiom<ListAccess>>::effect() const
{ return dynamic_cast<const EffectAxiom<ListAccess> &>(element()); }

EC_word Semantics<EffectAxiom<ListAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(nullptr, &effect().lhs());

	effect().scope().semantics().init_vars();

	EC_word condition_term = element().condition().semantics().plterm();
	if (fluent_access.first->semantics().args_need_eval()) {

		fluent_access.first->target()->scope().semantics().init_vars();

		condition_term = ::term(EC_functor("and", 2),
			fluent_access.first->semantics().args_binding(),
			condition_term
		);
	}

	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		fluent_access.first->semantics().plterm_free_args(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			effect().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		condition_term
	);
}




}
