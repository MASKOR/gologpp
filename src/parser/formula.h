#ifndef GOLOGPP_PARSER_FORMULA_H_
#define GOLOGPP_PARSER_FORMULA_H_

#include <model/formula.h>

#include "utilities.h"
#include "reference.h"
#include "arithmetic.h"
#include "symbolic_expression.h"
#include "string_expression.h"


namespace gologpp {
namespace parser {


struct ComparisonParser : grammar<Comparison *(Scope &), locals<Typename>> {
	ComparisonParser();

	rule<Comparison *(Scope &), locals<Typename>> comparison;
	rule<ComparisonOperator()> cmp_op;
	rule<Expression *(Scope &)> comparable_expr;
	rule<Expression *(Scope &, Typename)> typed_expr;
};



struct BooleanExpressionParser : grammar<Expression *(Scope &)> {
	BooleanExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> unary_expr;
	rule<Expression *(Scope &)> binary_expr;
	rule<Expression *(Scope &)> negation;
	rule<Expression *(Scope &)> brace;
	rule<Expression *(Scope &)> bool_var_ref;
	rule<Expression *(Scope &), locals<Scope *>> quantification;
	rule<QuantificationOperator()> quantification_op;
	rule<BooleanOperator()> bool_op;
	ReferenceParser<Fluent> boolean_fluent_ref;
	ReferenceParser<Function> boolean_function_ref;
	ComparisonParser comparison;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FORMULA_H_

