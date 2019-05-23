#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


rule<FieldAccess *(Scope &, Typename)> &field_access();

rule<FieldAccess *(Scope &)> &any_field_access();


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
