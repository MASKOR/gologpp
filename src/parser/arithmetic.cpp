#include <model/fluent.h>
#include <model/procedural.h>

#include "arithmetic.h"
#include "reference.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>

namespace gologpp {
namespace parser {

ExpressionParser<NumericExpression>::ExpressionParser()
	: ExpressionParser::base_type(expression, "numeric_expression")
	{
		expression = binary_expr(_r1) | unary_expr(_r1);
		expression.name("numeric_expression");

		unary_expr = brace(_r1) | constant<NumericExpression>() | num_var_ref(_r1)
			| num_fluent_ref(_r1) | num_function_ref(_r1);
		unary_expr.name("unary_numeric_expression");

		binary_expr = (
			(unary_expr(_r1) >> arith_operator) > expression(_r1)
		) [
			_val = new_<ArithmeticOperation>(at_c<0>(_1), at_c<1>(_1), _2)
		];
		binary_expr.name("binary_numeric_expression");

		brace = '(' >> expression(_r1) >> ')';
		brace.name("braced_numeric_expression");

		arith_operator =
			qi::string("+") [ _val = val(ArithmeticOperation::ADDITION) ]
			| qi::string("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
			| qi::string("/") [ _val = val(ArithmeticOperation::DIVISION) ]
			| qi::string("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
			| qi::string("**") [ _val = val(ArithmeticOperation::POWER) ]
			| qi::string("%") [ _val = val(ArithmeticOperation::MODULO) ]
		;
		arith_operator.name("arithmetic_operator");

		num_var_ref = var<NumericExpression>()(_r1) [
			_val = new_<Reference<NumericVariable>>(_1)
		];
		num_var_ref.name("reference_to_numeric_variable");

		BOOST_SPIRIT_DEBUG_NODES((expression)(binary_expr)(unary_expr)
			(operation)(brace)(num_var_ref)(arith_operator));
	}

} // namespace parser
} // namespace gologpp

