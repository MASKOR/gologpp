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


void define_expression_rules();

rule<Expression *(Scope &)> &value_expression();

rule<Expression *(Scope &, Typename)> &typed_expression();

extern rule<Expression *(Scope &)> boolean_expression;
extern rule<Expression *(Scope &)> numeric_expression;
extern rule<Expression *(Scope &)> string_expression;
extern rule<Expression *(Scope &)> symbolic_expression;
extern rule<Expression *(Scope &)> compound_expression;
extern rule<Expression *(Scope &)> list_expression;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EXPRESSIONS_H_

