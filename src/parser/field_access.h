#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FieldAccessParser : public grammar<FieldAccess<ExprT> *(Scope &)> {
	FieldAccessParser();

	rule<FieldAccess<ExprT> *(Scope &)> field_access;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
