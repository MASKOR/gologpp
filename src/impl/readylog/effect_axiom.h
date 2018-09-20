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


template<class ExpressionT>
class Semantics<EffectAxiom<ExpressionT>> : public ReadylogSemantics {
public:
	Semantics(const EffectAxiom<ExpressionT> &eff)
	: effect_(eff)
	{}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override
	{
		effect_.action().scope().semantics().init_vars();

		EC_ref Srest, Body_cond, Eval_val, Body_subf, New_body;

		if ( ! EclipseContext::instance().ec_query(
			::term(EC_functor("process_condition", 3),
				effect_.condition().semantics().plterm(),
				Srest,
				Body_cond
			)
			&& ::term(EC_functor("process_subf", 5),
				effect_.value().semantics().plterm(),
				Eval_val,
				Body_subf,
				::newvar(),
				Srest
			)
			&& ::term(EC_functor("conjunct", 3),
				Body_cond,
				Body_subf,
				New_body
			)
		))
			throw std::runtime_error("Failed to generate SSA for " + effect_.action().name());

		return ::term(EC_functor(":-", 2),
			::term(EC_functor("ssa", 3),
				effect_.fluent().semantics().plterm(),
				Eval_val,
				::list(effect_.action().semantics().plterm(), Srest)
			),
			New_body
		);
	}


private:
	const EffectAxiom<ExpressionT> &effect_;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
