#ifndef GOLOGPP_PARSER_LIST_ACCESS_H_
#define GOLOGPP_PARSER_LIST_ACCESS_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


rule<ListAccess *(Scope &, Expression *, Typename)> &single_list_access();

rule<ListAccess *(Scope &, Expression *, Typename)> &deep_list_access();

rule<Expression *(Scope &, Typename)> &mixed_list_access();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_LIST_ACCESS_H_
