#ifndef GOLOGPP_PARSER_ASSIGNMENT_H_
#define GOLOGPP_PARSER_ASSIGNMENT_H_

#include <model/fluent.h>
#include <model/procedural.h>

#include "utilities.h"
#include "reference.h"


namespace gologpp {
namespace parser {



template<class LhsT>
struct AssignmentParser : grammar<Assignment<LhsT> *(Scope &)> {
	AssignmentParser();

	void init();

	rule<Assignment<LhsT> *(Scope &)> assignment;
	rule<LhsT *(Scope &)> lhs_parser;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ASSIGNMENT_H_

