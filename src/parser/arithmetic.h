#ifndef GOLOGPP_PARSER_ARITHMETIC_H_
#define GOLOGPP_PARSER_ARITHMETIC_H_

#include "utilities.h"
#include "atoms.h"
#include "field_access.h"
#include "reference.h"

#include <model/arithmetic.h>


namespace gologpp {
namespace parser {

/*********************
* Numeric expressions
*********************/

struct NumericExpressionParser : grammar<Expression *(Scope &)> {
	NumericExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> binary_expr;
	rule<Expression *(Scope &)> operation;
	rule<Expression *(Scope &)> brace;
	rule<Expression *(Scope &)> num_var_ref;
	rule<Expression *(Scope &)> unary_expr;
	rule<ArithmeticOperation::Operator()> arith_operator;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ARITHMETIC_H_

