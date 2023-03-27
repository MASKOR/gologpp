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

#pragma once

#include <model/platform/clock_formula.h>
#include <semantics/platform/taptenc/semantics.h>
#include <taptenc/src/constraints/constraints.h>


namespace gologpp {


template<>
class Semantics<platform::ClockFormula>
: public TaptencExpressionSemantics
{
public:
	virtual std::unique_ptr<taptenc::ClockConstraint> compile() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::ClockBound>
: public GeneralSemantics<platform::ClockBound>
, public Semantics<platform::ClockFormula>
{
public:
	using GeneralSemantics<platform::ClockBound>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::ClockConstraint> compile() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::BooleanClockOperation>
: public GeneralSemantics<platform::BooleanClockOperation>
, public Semantics<platform::ClockFormula>
{
public:
	using GeneralSemantics<platform::BooleanClockOperation>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::ClockConstraint> compile() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::ClockNegation>
: public GeneralSemantics<platform::ClockNegation>
, public Semantics<platform::ClockFormula>
{
public:
	using GeneralSemantics<platform::ClockNegation>::GeneralSemantics;

	virtual std::unique_ptr<taptenc::ClockConstraint> compile() override;
};


} // namespace gologpp
