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
	return ::term(EC_functor(cv_functor.c_str(), 4),
		effect().action().semantics().plterm(),
		effect().lhs().subject().semantics().plterm(),
		effect().lhs().semantics().field_assign(effect().value()),
		effect().condition().semantics().plterm()
	);
}

}
