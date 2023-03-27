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

#include "arithmetic.h"
#include "variable.h"
#include "value.h"
#include "list.h"
#include "mixed_member_access.h"
#include "expressions.h"
#include "reference.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_plus.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

namespace gologpp {
namespace parser {



NumericExpressionParser::NumericExpressionParser()
: NumericExpressionParser::base_type(expression, "numeric_expression")
{
	expression = binary_expr(_r1) | unary_expr(_r1);
	expression.name("numeric_expression");

	unary_expr = brace(_r1) | numeric_value()
		| var_ref()(_r1, number_type())
		| list_length(_r1)
		| conditional_expression(_r1, number_type())
		| typed_reference<Fluent>()(_r1, number_type())
		| typed_reference<Function>()(_r1, number_type())
		| typed_reference<ExogFunction>()(_r1, number_type())
		| mixed_member_access()(_r1, number_type())
	;
	unary_expr.name("unary_numeric_expression");

	list_length = (lit("length") > '(' > list_expression(_r1, undefined_type()) > ')') [
		_val = new_<ListLength>(_1)
	];

	binary_expr = (
		+(unary_expr(_r1) >> arith_operator) > unary_expr(_r1)
	) [
		_val = phoenix::bind(&parse_op_precedence<ArithmeticOperation>, _1, _2)
	];
	binary_expr.name("binary_numeric_expression");

	brace = '(' >> expression(_r1) >> ')';
	brace.name("braced_numeric_expression");

	arith_operator =
		lit("+") [ _val = val(ArithmeticOperation::ADDITION) ]
		| lit("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
		| lit("/") [ _val = val(ArithmeticOperation::DIVISION) ]
		| lit("**") [ _val = val(ArithmeticOperation::POWER) ]
		| lit("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
		| lit("%") [ _val = val(ArithmeticOperation::MODULO) ]
	;
	arith_operator.name("arithmetic_operator");

	GOLOGPP_DEBUG_NODES(
		//(expression)(binary_expr)(unary_expr)
		//(operation)(brace)(num_var_ref)(arith_operator)
	)
}



} // namespace parser
} // namespace gologpp
