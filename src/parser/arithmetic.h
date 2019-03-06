#ifndef GOLOGPP_PARSER_ARITHMETIC_H_
#define GOLOGPP_PARSER_ARITHMETIC_H_

#include "utilities.h"
#include "expressions.h"
#include "atoms.h"

#include <model/arithmetic.h>


namespace gologpp {
namespace parser {

/*********************
* Numeric expressions
*********************/


extern ReferenceParser<NumericFluent> numeric_fluent_ref;
extern ReferenceParser<NumericFunction> numeric_function_ref;
extern FieldAccessParser<NumericExpression> numeric_field_access;
extern ConstantParser<NumericExpression> numeric_constant;

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
};

extern ExpressionParser<NumericExpression> numeric_expression;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ARITHMETIC_H_

