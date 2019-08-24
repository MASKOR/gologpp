#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "reference.h"


namespace gologpp {
namespace parser {


extern rule<Expression *(Scope &)> compound_atom;

extern rule<Expression *(Scope &)> compound_expression;


void initialize_compound_exprs();


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
