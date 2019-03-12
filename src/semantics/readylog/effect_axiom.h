#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include "semantics.h"

#include <model/effect_axiom.h>
#include "reference.h"
#include <model/action.h>
#include <model/fluent.h>
#include "scope.h"
#include "execution.h"

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<AbstractEffectAxiom> : public ReadylogSemantics {
public:
	Semantics(const AbstractEffectAxiom &eff);

protected:
	const AbstractEffectAxiom &effect_;
	string cv_functor;
};


template<class ExprT>
class Semantics<EffectAxiom<Reference<Fluent<ExprT>>>> : public Semantics<AbstractEffectAxiom> {
public:
	using Semantics<AbstractEffectAxiom>::Semantics;

	const EffectAxiom<Reference<Fluent<ExprT>>> &effect() const
	{ return dynamic_cast<const EffectAxiom<Reference<Fluent<ExprT>>> &>(effect_); }

	virtual EC_word plterm() override
	{
		return ::term(EC_functor(cv_functor.c_str(), 4),
			effect().action().semantics().plterm(),
			effect().lhs().semantics().plterm(),
			effect().value().semantics().plterm(),
			effect().condition().semantics().plterm()
		);
	}
};


template<class ExprT>
class Semantics<EffectAxiom<FieldAccess<ExprT>>> : public Semantics<AbstractEffectAxiom> {
	const EffectAxiom<FieldAccess<ExprT>> &effect() const
	{ return dynamic_cast<const EffectAxiom<FieldAccess<ExprT>> &>(effect_); }

	virtual EC_word plterm() override
	{
		return ::term(EC_functor(cv_functor.c_str(), 4),
			effect().action().semantics().plterm(),
			effect().lhs().subject().semantics().plterm(),
			effect().lhs().semantics().field_assign(effect().value()),
			effect().condition().semantics().plterm()
		);
	}
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
