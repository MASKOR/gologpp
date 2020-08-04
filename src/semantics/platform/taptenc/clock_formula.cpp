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

#include "clock_formula.h"
#include <model/platform/clock_formula.h>
#include <semantics/platform/taptenc/reference.h>


namespace gologpp {


std::unique_ptr<taptenc::ClockConstraint> Semantics<platform::ClockBound>::compile()
{
	taptenc::ComparisonOp op;
	switch (element().op()) {
	case platform::ClockBound::Operator::GE:
		op = taptenc::ComparisonOp::GTE;
		break;
	case platform::ClockBound::Operator::LE:
		op = taptenc::ComparisonOp::LTE;
	}

	return std::unique_ptr<taptenc::ClockConstraint>(new taptenc::ComparisonCC(
		element().clock().semantics().compile(),
		op,
		element().bound().numeric_convert<int>()
	));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::ClockConstraint> Semantics<platform::BooleanClockOperation>::compile()
{
	if (element().op() != platform::BooleanClockOperation::Operator::AND)
		throw EngineError("Only conjunction of clock constraints is supported by the taptenc semantics");

	return std::unique_ptr<taptenc::ClockConstraint>(new taptenc::ConjunctionCC(
		*dynamic_cast<ClockFormulaSemantics &>(element().lhs().semantics()).compile(),
		*dynamic_cast<ClockFormulaSemantics &>(element().rhs().semantics()).compile()
	));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<taptenc::ClockConstraint> Semantics<platform::ClockNegation>::compile()
{ throw EngineError("Negation of clock constraints is not supported by the taptenc semantics"); }


} // namespace gologpp
