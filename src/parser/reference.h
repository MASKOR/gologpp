#ifndef GOLOGPP_PARSER_REFERENCE_H_
#define GOLOGPP_PARSER_REFERENCE_H_

#include "utilities.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"
#include "formula.h"
#include "arithmetic.h"


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
	ExpressionParser<BooleanExpression> boolean_expr;
	ExpressionParser<NumericExpression> numeric_expr;
	ExpressionParser<StringExpression> string_expr;
	ExpressionParser<SymbolicExpression> symbolic_expr;
	ExpressionParser<CompoundExpression> compound_expr;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_REFERENCE_H_

