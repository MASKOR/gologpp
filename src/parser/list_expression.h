#ifndef GOLOGPP_PARSER_LIST_EXPRESSION_H_
#define GOLOGPP_PARSER_LIST_EXPRESSION_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


extern rule<Expression *(Scope &)> list_atom;

extern rule<Expression *(Scope &)> list_expression;


void initialize_list_exprs();


} // namespace parser
} // namespace gologpp



#endif // GOLOGPP_PARSER_LIST_EXPRESSION_H_
