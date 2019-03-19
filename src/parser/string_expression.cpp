#include "string_expression.h"
#include "arithmetic.h"
#include "formula.h"
#include "symbolic_expression.h"

#include "atoms.h"

#include <model/fluent.h>
#include <model/procedural.h>
#include <model/arithmetic.h>
#include <model/string.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_char_.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {


ExpressionParser<StringExpression> &string_expression_()
{
	static ExpressionParser<StringExpression> rv;
	return rv;
}

rule<StringExpression *(Scope &)> string_expression = string_expression_()(_r1);


ExpressionParser<StringExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "string_expression")
{
	expression = concatenation(_r1) | unary_expr(_r1);
	expression.name("string_expression");

	nested_expression = concatenation(_r1) | implicit_conversion(_r1) | nested_unary(_r1);

	nested_unary = unary_expr(_r1)
		| any_simple_expr(_r1) [
			_val = new_<ToString>(_1)
		];

	unary_expr = string_constant | var_ref(_r1) | string_fluent_ref(_r1)
		| string_function_ref(_r1)
		| string_field_access(_r1)
		| conversion(_r1);
	unary_expr.name("unary_string_expression");

	var_ref = var<StringExpression>()(_r1) [
		_val = new_<Reference<Variable<StringExpression>>>(_1)
	];
	var_ref.name("reference_to_string_variable");

	conversion = ( lit("to_string") > '(' > any_simple_expr(_r1) > ')' ) [
		_val = new_<ToString>(_1)
	];

	any_simple_expr =
		numeric_expression(_r1)
		| boolean_expression(_r1)
		| symbolic_expression(_r1);
	any_simple_expr.name("simple_value_expression");


	concatenation = (unary_expr(_r1) >> "+" >> nested_expression(_r1)) [
		_val = new_<StringConcatenation>(_1, _2)
	];
	concatenation.name("string_concatenation");

	implicit_conversion = ( any_simple_expr(_r1) >> '+' >> expression(_r1) ) [
		_val = new_<StringConcatenation>(_1, _2)
	];
	implicit_conversion.name("implicit_string_conversion");

	GOLOGPP_DEBUG_NODES(
		(expression)(nested_expression)(nested_unary)(unary_expr)(var_ref)(conversion)
		(any_simple_expr)(concatenation)(implicit_conversion)
	);
}


} // namespace parser
} // namespace gologpp
