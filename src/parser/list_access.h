#ifndef GOLOGPP_PARSER_LIST_ACCESS_H_
#define GOLOGPP_PARSER_LIST_ACCESS_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


rule<Expression *(Scope &, Typename)> &list_access();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_LIST_ACCESS_H_
