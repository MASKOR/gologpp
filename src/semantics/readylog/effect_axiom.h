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
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
		try {
			dynamic_cast<const Action &>(effect_.action());
			// All golog++ actions are implicitly durative
			cv_functor = "durative_causes_val";
		} catch (std::bad_cast &) {
			try {
				dynamic_cast<const ExogAction &>(effect_.action());
				cv_functor = "causes_val";
			} catch (std::bad_cast &) {
				throw Bug("Unknown action type");
			}
		}
#pragma GCC diagnostic pop
	}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override
	{
		return ::term(EC_functor(cv_functor.c_str(), 4),
			effect_.action().semantics().plterm(),
			effect_.fluent().semantics().plterm(),
			effect_.value().semantics().plterm(),
			effect_.condition().semantics().plterm()
		);
	}

	/*EC_word ssa()
	{
		effect_.action().scope().semantics().init_vars();

		EC_ref Srest, Body_cond, Eval_val, Body_subf, New_body;

		if ( ! ReadylogContext::instance().ec_query(
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
	}*/


private:
	const EffectAxiom<ExpressionT> &effect_;
	string cv_functor;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
