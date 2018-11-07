#ifndef GOLOGPP_PARSER_H_
#define GOLOGPP_PARSER_H_

#include <model/gologpp.h>

namespace gologpp {
namespace parser {


unique_ptr<VoidExpression> parse_string(const std::string &code);

unique_ptr<VoidExpression> parse_file(const std::string &filename);


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_H_
