#ifndef GOLOGPP_PARSER_EXPRESSIONS_H_
#define GOLOGPP_PARSER_EXPRESSIONS_H_

#include <model/expressions.h>
#include "utilities.h"

namespace gologpp {
namespace parser {


/****************************
* General expressions:
* Just an undefined template
****************************/


extern rule<Expression *(Scope &)> value_expression;

extern rule<Expression *(Scope &, Typename)> typed_expression;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EXPRESSIONS_H_

