#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include "Implementation.h"

#include <gologpp/EffectAxiom.h>
#include "Reference.h"
#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include "Scope.h"

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
		effect_.action().parent_scope().implementation().init_vars();
		return ::term(
			EC_functor("causes_val", 3),
			effect_.action().implementation().term(),
			effect_.fluent().implementation().term(),
			effect_.value().implementation().term()
		);
	}


private:
	const EffectAxiom<ExpressionT> &effect_;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
