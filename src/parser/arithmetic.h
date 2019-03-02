#ifndef GOLOGPP_PARSER_ARITHMETIC_H_
#define GOLOGPP_PARSER_ARITHMETIC_H_

#include "utilities.h"
#include "expressions.h"

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
	unique_ptr<ReferenceParser<NumericFluent>> num_fluent_ref;
	unique_ptr<ReferenceParser<NumericFunction>> num_function_ref;
	unique_ptr<FieldAccessParser<NumericExpression>> field_access;
};




} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ARITHMETIC_H_

