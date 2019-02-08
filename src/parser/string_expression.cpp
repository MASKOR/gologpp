#include "string_expression.h"

#include "atoms.h"

#include <model/fluent.h>
#include <model/procedural.h>
#include <model/arithmetic.h>
#include <model/string_concatenation.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/phoenix/object/new.hpp>

namespace gologpp {
namespace parser {


ExpressionParser<StringExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "string_expression")
{
	expression = concatenation(_r1) | unary_expr(_r1);
	expression.name("string_expression");

	unary_expr = var_ref(_r1) | fluent_ref(_r1)
		| function_ref(_r1) | constant<StringExpression>();
	unary_expr.name("unary_string_expression");

	var_ref = var<StringExpression>()(_r1) [
		_val = new_<Reference<Variable<StringExpression>>>(_1)
	];
	var_ref.name("reference_to_string_variable");


	concatenation = (unary_expr(_r1) >> "+" >> expression(_r1)) [
		_val = new_<StringConcatenation>(_1, _2)
	];
	concatenation.name("string_concatenation");
}


} // namespace parser
} // namespace gologpp
