#ifndef GOLOGPP_PARSER_REFERENCE_H_
#define GOLOGPP_PARSER_REFERENCE_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


/******************
* References
******************/

template<class GologT>
struct ReferenceParser : grammar<Reference<GologT> *(Scope &)> {
	ReferenceParser();

	rule<Reference<GologT> *(Scope &)> pred_ref;
	rule<Expression *(Scope &)> any_expr;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_REFERENCE_H_

