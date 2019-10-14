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
#include "atoms.h"
#include "list_access.h"
#include "list_expression.h"
#include "field_access.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>

namespace gologpp {
namespace parser {



NumericExpressionParser::NumericExpressionParser()
: NumericExpressionParser::base_type(expression, "numeric_expression")
{
	expression = binary_expr(_r1) | unary_expr(_r1);
	expression.name("numeric_expression");

	unary_expr = brace(_r1) | numeric_literal()
		| num_var_ref(_r1)
		| list_length(_r1)
		| typed_reference<Fluent>()(_r1, NumberType::name())
		| typed_reference<Function>()(_r1, NumberType::name())
		| mixed_list_access()(_r1, NumberType::name())
		| mixed_field_access()(_r1, NumberType::name())
	;
	unary_expr.name("unary_numeric_expression");

	list_length = (lit("length") > '(' > list_expression(_r1) > ')') [
		_val = new_<ListLength>(_1)
	];

	binary_expr = (
		(unary_expr(_r1) >> arith_operator) > expression(_r1)
	) [
		_val = new_<ArithmeticOperation>(at_c<0>(_1), at_c<1>(_1), _2)
	];
	binary_expr.name("binary_numeric_expression");

	brace = '(' >> expression(_r1) >> ')';
	brace.name("braced_numeric_expression");

	arith_operator =
		lit("+") [ _val = val(ArithmeticOperation::ADDITION) ]
		| lit("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
		| lit("/") [ _val = val(ArithmeticOperation::DIVISION) ]
		| lit("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
		| lit("**") [ _val = val(ArithmeticOperation::POWER) ]
		| lit("%") [ _val = val(ArithmeticOperation::MODULO) ]
	;
	arith_operator.name("arithmetic_operator");

	num_var_ref = var_usage()(_r1, NumberType::name()) [
		_val = new_<Reference<Variable>>(_1)
	];
	num_var_ref.name("reference_to_numeric_variable");

	GOLOGPP_DEBUG_NODES(
		//(expression)(binary_expr)(unary_expr)
		//(operation)(brace)(num_var_ref)(arith_operator)
	)
}



} // namespace parser
} // namespace gologpp
