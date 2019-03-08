#ifndef GOLOGPP_PARSER_FIELD_ACCESS_H_
#define GOLOGPP_PARSER_FIELD_ACCESS_H_

#include "utilities.h"
#include "reference.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FieldAccessParser : public grammar<FieldAccess<ExprT> *(Scope &)> {
	FieldAccessParser();

	rule<FieldAccess<ExprT> *(Scope &)> field_access;
	rule<CompoundExpression *(Scope &)> unary_compound_expr;
	ReferenceParser<Fluent<CompoundExpression>> compound_fluent_ref;
	ReferenceParser<Function<CompoundExpression>> compound_function_ref;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_FIELD_ACCESS_H_
