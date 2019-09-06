#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


extern rule<string()> field_access;

rule<Expression *(Scope &, Typename)> &mixed_field_access();


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
