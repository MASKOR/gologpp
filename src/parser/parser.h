#ifndef GOLOGPP_PARSER_H_
#define GOLOGPP_PARSER_H_

#include <model/gologpp.h>

namespace gologpp {
namespace parser {


shared_ptr<Scope> parse_string(const std::string &code);


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_H_