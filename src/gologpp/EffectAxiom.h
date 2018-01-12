#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "Formula.h"
#include "Fluent.h"
#include "Execution.h"
#include "Translation.h"

namespace gologpp {

class EffectAxiom {
public:
	EffectAxiom(const shared_ptr<Fluent> fluent, const shared_ptr<Expression> value);
	EffectAxiom(EffectAxiom &&);
	virtual ~EffectAxiom();

protected:
	shared_ptr<Fluent> fluent_;
	shared_ptr<Expression> value_;
};


} /* namespace gologpp */

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
