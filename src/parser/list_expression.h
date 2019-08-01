#ifndef GOLOGPP_PARSER_LIST_EXPRESSION_H_
#define GOLOGPP_PARSER_LIST_EXPRESSION_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


struct ListExpressionParser : grammar<Expression *(Scope &)> {
	ListExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> list_expression;
	rule<Reference<Variable> *(Scope &)> var_ref;
};


} // namespace parser
} // namespace gologpp



#endif // GOLOGPP_PARSER_LIST_EXPRESSION_H_
