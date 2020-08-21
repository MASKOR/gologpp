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

#include <parser/value.h>
#include <parser/platform/reference.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

namespace gologpp {
namespace parser {



ClockFormulaParser::ClockFormulaParser()
: ClockFormulaParser::base_type(clock_formula, "clock_formula")
{
	clock_formula = clock_operation_seq(_r1) | clock_unary(_r1);
	clock_formula.name("clock_formula");

	clock_unary = clock_braced(_r1)
		| clock_negation(_r1)
		| clock_bound(_r1)
	;
	clock_unary.name("clock_unary");

	clock_operation_seq = (
		+(clock_unary(_r1) >> clock_binary_op) >> clock_unary(_r1)
	) [
		_val = phoenix::bind(&parse_op_precedence<platform::BooleanClockOperation, Expression>, _1, _2)
	];
	clock_operation_seq.name("clock_operation_seq");

	clock_binary_op = lit('&') [ _val = platform::BooleanClockOperation::Operator::AND ]
		| lit('|') [ _val = platform::BooleanClockOperation::Operator::OR ];
	clock_binary_op.name("clock_binary_operator");

	clock_braced = lit('(') > clock_formula(_r1) > ')';
	clock_braced.name("clock_braced_formula");

	clock_bound = (
		platform_ref<platform::Clock>()(phoenix::bind(&platform::Component::m_scope, _r1))
		> clock_bound_operator > numeric_value()) [
			_val = new_<platform::ClockBound>(_1, _2, _3)
		];
	clock_bound.name("clock_bound");

	clock_bound_operator = lit(">=") [ _val = platform::ClockBound::Operator::GE ]
		| lit("<=") [ _val = platform::ClockBound::Operator::LE ];
	clock_bound_operator.name("clock_bound_operator");

	clock_negation = lit('!') > clock_unary(_r1) [
		_val = new_<platform::ClockNegation>(_1)
	];
	clock_negation.name("clock_negation");

	GOLOGPP_DEBUG_NODES(
		(clock_formula)
		(clock_unary)
		(clock_negation)
		(clock_bound)
		(clock_operation_seq)
		(clock_braced)
		(clock_bound)
	)
}


} // namespace parser
} // namespace gologpp
