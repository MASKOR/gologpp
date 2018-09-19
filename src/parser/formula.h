#ifndef GOLOGPP_PARSER_FORMULA_H_
#define GOLOGPP_PARSER_FORMULA_H_

#include <model/formula.h>

#include "utilities.h"
#include "expressions.h"
#include "arithmetic.h"
#include "reference.h"


namespace gologpp {
namespace parser {

template<>
struct ExpressionParser<BooleanExpression> : grammar<BooleanExpression *(Scope &)> {
	ExpressionParser();

	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> unary_expr;
	rule<BooleanExpression *(Scope &)> binary_expr;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
	rule<BooleanExpression *(Scope &)> bool_var_ref;
	rule<BooleanExpression *(Scope &), locals<Scope *>> quantification;
	rule<QuantificationOperator()> quantification_op;
	ReferenceParser<BooleanFluent> bool_fluent_ref;
	ReferenceParser<BooleanFunction> bool_function_ref;
	rule<BooleanOperator()> bool_op;
	rule<BooleanExpression *(Scope &)> num_comparison;
	NumericExpressionParser num_expression;
	rule<ComparisonOperator()> num_cmp_op;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FORMULA_H_

