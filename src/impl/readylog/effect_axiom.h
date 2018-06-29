#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include "implementation.h"

#include <model/effect_axiom.h>
#include "reference.h"
#include <model/action.h>
#include <model/fluent.h>
#include "scope.h"
#include "execution.h"

#include <eclipseclass.h>


namespace gologpp {


template<class ExpressionT>
class Implementation<EffectAxiom<ExpressionT>> : public ReadylogImplementation {
public:
	Implementation(const EffectAxiom<ExpressionT> &eff)
	: effect_(eff)
	{}

	virtual ~Implementation() override = default;

	virtual EC_word term() override
	{
		effect_.action()->scope().implementation().init_vars();

		EC_ref Srest, Body_cond, Eval_val, Body_subf, New_body;

		if ( ! EclipseContext::instance().ec_query(
			::term(EC_functor("process_condition", 3),
				effect_.condition().implementation().term(),
				Srest,
				Body_cond
			)
			&& ::term(EC_functor("process_subf", 5),
				effect_.value().implementation().term(),
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
			throw std::runtime_error("Failed to generate SSA for " + effect_.action()->name());

		return ::term(EC_functor(":-", 2),
			::term(EC_functor("ssa", 3),
				effect_.fluent().implementation().term(),
				Eval_val,
				::list(effect_.action()->implementation().term(), Srest)
			),
			New_body
		);
	}


private:
	const EffectAxiom<ExpressionT> &effect_;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
