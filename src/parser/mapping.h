#ifndef GOLOGPP_PARSER_MAPPING_H_
#define GOLOGPP_PARSER_MAPPING_H_

#include "utilities.h"

#include <model/mapping.h>


namespace gologpp {
namespace parser {


struct MappingParser : public grammar<ActionMapping *(Scope &)> {
	MappingParser();

	rule<ActionMapping *(Scope &)> mapping;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_MAPPING_H_