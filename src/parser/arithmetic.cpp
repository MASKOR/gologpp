#include "arithmetic.h"
#include "atoms.h"

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



ExpressionParser<NumericExpression> &numeric_expression_()
{
	static ExpressionParser<NumericExpression> numeric_expression_;
	return numeric_expression_;
}

rule<NumericExpression *(Scope &)> numeric_expression = numeric_expression_()(_r1);


ExpressionParser<NumericExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "numeric_expression")
{
	expression = binary_expr(_r1) | unary_expr(_r1);
	expression.name("numeric_expression");

	unary_expr = brace(_r1) | numeric_constant
		| num_var_ref(_r1)
		| numeric_fluent_ref(_r1) | numeric_function_ref(_r1)
		| numeric_field_access(_r1)
	;
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
		lit("+") [ _val = val(ArithmeticOperation::ADDITION) ]
		| lit("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
		| lit("/") [ _val = val(ArithmeticOperation::DIVISION) ]
		| lit("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
		| lit("**") [ _val = val(ArithmeticOperation::POWER) ]
		| lit("%") [ _val = val(ArithmeticOperation::MODULO) ]
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
