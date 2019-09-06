#include "effect_axiom.h"
#include "procedural.h"



namespace gologpp {


Semantics<AbstractEffectAxiom>::Semantics(const AbstractEffectAxiom &eff)
: effect_(eff)
{
	if (effect_.action().is_a<Action>())
		// All golog++ actions are implicitly durative
		cv_functor = "durative_causes_val";
	else if (effect_.action().is_a<ExogAction>())
		cv_functor = "causes_val";
	else
		throw Bug("Unknown action type");
}


const EffectAxiom<Reference<Fluent>> &Semantics<EffectAxiom<Reference<Fluent>>>::effect() const
{ return dynamic_cast<const EffectAxiom<Reference<Fluent>> &>(effect_); }


EC_word Semantics<EffectAxiom<Reference<Fluent>>>::plterm()
{
	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		effect().lhs().semantics().plterm(),
		effect().value().semantics().plterm(),
		effect().condition().semantics().plterm()
	);
}


const EffectAxiom<FieldAccess> &Semantics<EffectAxiom<FieldAccess>>::effect() const
{ return dynamic_cast<const EffectAxiom<FieldAccess> &>(effect_); }

EC_word Semantics<EffectAxiom<FieldAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(&effect().lhs(), nullptr);

	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			effect().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		effect().condition().semantics().plterm()
	);
}


const EffectAxiom<ListAccess> &Semantics<EffectAxiom<ListAccess>>::effect() const
{ return dynamic_cast<const EffectAxiom<ListAccess> &>(effect_); }

EC_word Semantics<EffectAxiom<ListAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(nullptr, &effect().lhs());

	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			effect().value().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		),
		effect().condition().semantics().plterm()
	);
}




}
