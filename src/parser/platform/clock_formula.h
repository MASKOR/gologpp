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

#include <parser/utilities.h>

#include <model/platform/clock_formula.h>

namespace gologpp {
namespace parser {


struct ClockFormulaParser : public grammar<platform::ClockFormula *(platform::Component &)> {
	ClockFormulaParser();

	rule<platform::ClockFormula *(platform::Component &)> clock_formula;
	rule<platform::ClockFormula *(platform::Component &)> clock_unary;
	rule<platform::BooleanClockOperation::Operator()> clock_binary_op;
	rule<platform::ClockFormula *(platform::Component &)> clock_operation_seq;
	rule<platform::ClockFormula *(platform::Component &)> clock_braced;
	rule<platform::ClockFormula *(platform::Component &)> clock_bound;
	rule<platform::ClockBound::Operator()> clock_bound_operator;
	rule<platform::ClockFormula *(platform::Component &)> clock_negation;
};


} // namespace parser
} // namespace gologpp
