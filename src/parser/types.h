#ifndef GOLOGPP_PARSER_TYPES_H_
#define GOLOGPP_PARSER_TYPES_H_

#include <model/gologpp.h>
#include <model/types.h>

#include "utilities.h"


namespace gologpp {
namespace parser {


struct CompoundTypeParser : public grammar<CompoundType *(Scope &)> {
	CompoundTypeParser();

	rule<CompoundType *(Scope &)> type_definition;
};


extern rule<Typename()> any_type_specifier;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_TYPES_H_
