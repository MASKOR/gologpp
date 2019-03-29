#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"
#include "compound_expression.h"
#include "reference.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FieldAccessParser : public grammar<FieldAccess<ExprT> *(Scope &)> {
	FieldAccessParser();

	rule<FieldAccess<ExprT> *(Scope &)> field_access;
	ExpressionParser<CompoundExpression> compound_expr;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
