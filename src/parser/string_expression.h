#ifndef GOLOGPP_PARSER_STRING_EXPRESSION_H_
#define GOLOGPP_PARSER_STRING_EXPRESSION_H_

#include "utilities.h"
#include "reference.h"

namespace gologpp {
namespace parser {


struct StringExpressionParser : grammar<Expression *(Scope &)> {
	StringExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> unary_expr;
	rule<Reference<Variable> *(Scope &)> var_ref;
	rule<Expression *(Scope &)> concatenation;
	rule<Expression *(Scope &)> conversion;
	rule<Expression *(Scope &)> convertible_expr;
	rule<Expression *(Scope &)> implicit_conversion;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_STRING_EXPRESSION_H_
