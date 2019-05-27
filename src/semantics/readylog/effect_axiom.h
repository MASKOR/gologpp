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


template<>
class Semantics<EffectAxiom<Reference<Fluent>>> : public Semantics<AbstractEffectAxiom> {
public:
	using Semantics<AbstractEffectAxiom>::Semantics;

	const EffectAxiom<Reference<Fluent>> &effect() const;
	virtual EC_word plterm() override;
};


template<>
class Semantics<EffectAxiom<FieldAccess>> : public Semantics<AbstractEffectAxiom> {
	using Semantics<AbstractEffectAxiom>::Semantics;

	const EffectAxiom<FieldAccess> &effect() const;
	virtual EC_word plterm() override;
};


} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
