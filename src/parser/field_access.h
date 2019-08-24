#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


rule<FieldAccess *(Expression *, Typename)> &single_field_access();

rule<FieldAccess *(Expression *, Typename)> &deep_field_access();

rule<Expression *(Scope &, Typename)> &mixed_field_access();


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
