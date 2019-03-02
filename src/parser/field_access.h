#include "utilities.h"
#include "compound_expression.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FieldAccessParser : public grammar<FieldAccess<ExprT> *(Scope &)> {
	FieldAccessParser();

	ExpressionParser<CompoundExpression> compound_expr;
	rule<FieldAccess<ExprT> *(Scope &)> field_access;
};


} // namespace parser
} // namespace gologpp