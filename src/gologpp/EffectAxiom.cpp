/*
 * SuccessorStateAxiom.cpp
 *
 *  Created on: 08.04.2016
 *      Author: vmatare
 */

#include "EffectAxiom.h"

#include "error.h"

#include <iostream>

namespace gologpp {

EffectAxiom::EffectAxiom(const shared_ptr<Fluent> fluent, const shared_ptr<Expression> value)
: fluent_(fluent)
, value_(value)
{}

EffectAxiom::EffectAxiom(EffectAxiom &&e)
: fluent_(std::move(e.fluent_))
, value_(std::move(e.value_))
{}

EffectAxiom::~EffectAxiom() {
}



} /* namespace gologpp */
