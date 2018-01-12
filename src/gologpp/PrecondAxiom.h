/*
 * PrecondAxiom.h
 *
 *  Created on: 08.04.2016
 *      Author: vmatare
 */

#ifndef GOLOGPP_PRECONDAXIOM_H_
#define GOLOGPP_PRECONDAXIOM_H_

#include "Formula.h"
#include "Execution.h"
#include "Translation.h"

#include <vector>

namespace gologpp {

class PrecondAxiom {
public:
	PrecondAxiom(unique_ptr<Expression> &&condition);
	PrecondAxiom(PrecondAxiom &&p);
	PrecondAxiom();
	~PrecondAxiom();

protected:
	unique_ptr<Expression> condition_;
};


} /* namespace gologpp */

#endif /* GOLOGPP_PRECONDAXIOM_H_ */
