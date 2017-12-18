/*
 * SuccessorStateAxiom.h
 *
 *  Created on: 08.04.2016
 *      Author: vmatare
 */

#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "Formula.h"
#include "Fluent.h"
#include "Execution.h"

namespace gologpp {

class EffectAxiom : public LanguageElement {
public:
	EffectAxiom(const shared_ptr<Fluent> fluent, const shared_ptr<Expression> value);
	EffectAxiom(EffectAxiom &&);
	~EffectAxiom();

private:
	shared_ptr<Fluent> fluent_;
	shared_ptr<Expression> value_;
};

} /* namespace gologpp */

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
