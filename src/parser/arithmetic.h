#ifndef GOLOGPP_PARSER_ARITHMETIC_H_
#define GOLOGPP_PARSER_ARITHMETIC_H_

#include "utilities.h"
#include "expressions.h"
#include "atoms.h"
#include "field_access.h"
#include "reference.h"

#include <model/arithmetic.h>


namespace gologpp {
namespace parser {

/*********************
* Numeric expressions
*********************/

template<>
struct ExpressionParser<NumericExpression> : grammar<NumericExpression *(Scope &)> {
	ExpressionParser();

	rule<NumericExpression *(Scope &)> expression;
	rule<NumericExpression *(Scope &)> binary_expr;
	rule<NumericExpression *(Scope &)> operation;
	rule<NumericExpression *(Scope &)> brace;
	rule<NumericExpression *(Scope &)> num_var_ref;
	rule<NumericExpression *(Scope &)> unary_expr;
	rule<ArithmeticOperation::Operator()> arith_operator;
	ReferenceParser<NumericFluent> numeric_fluent_ref;
	ReferenceParser<NumericFunction> numeric_function_ref;
	FieldAccessParser<NumericExpression> numeric_field_access;
};

extern rule<NumericExpression *(Scope &)> numeric_expression;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ARITHMETIC_H_

