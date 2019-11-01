/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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
class Semantics<AbstractEffectAxiom>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<AbstractEffectAxiom>
{
public:
	Semantics(const AbstractEffectAxiom &eff);

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


template<>
class Semantics<EffectAxiom<ListAccess>> : public Semantics<AbstractEffectAxiom> {
	using Semantics<AbstractEffectAxiom>::Semantics;

	const EffectAxiom<ListAccess> &effect() const;
	virtual EC_word plterm() override;
};



} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
