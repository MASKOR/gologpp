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
#include <model/action.h>
#include <model/fluent.h>

#include "reference.h"
#include "scope.h"
#include "execution.h"

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<AbstractEffectAxiom>
: public ReadylogSemantics
{
};


template<class LhsT>
class Semantics<EffectAxiom<LhsT>>
: public Semantics<AbstractEffectAxiom>
, public GeneralSemantics<EffectAxiom<LhsT>>
{
public:
	using GeneralSemantics<EffectAxiom<LhsT>>::GeneralSemantics;

	virtual EC_word plterm() override;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
