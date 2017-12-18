/*
 * PrecondAxiom.cpp
 *
 *  Created on: 08.04.2016
 *      Author: vmatare
 */

#include "PrecondAxiom.h"


namespace gologpp {

using namespace std;

PrecondAxiom::PrecondAxiom(unique_ptr<Expression> &&condition)
: condition_(std::move(condition))
{}


PrecondAxiom::PrecondAxiom(PrecondAxiom &&p)
: condition_(std::move(p.condition_))
{}


PrecondAxiom::~PrecondAxiom()
{}


} /* namespace gologpp */
