#include "expressions.h"

#include "arithmetic.h"
#include "formula.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"

namespace gologpp {
namespace parser {


template<class ExprT>
rule<ExprT *(Scope &)> &expression()
{
	static ExpressionParser<ExprT> p;
	static rule<ExprT *(Scope &)> rv { p(_r1) };
	return rv;
}

#define GOLOGPP_INSTANTIATE_EXPRESSION_RULE(_r, _data, T) \
	template \
	rule<T *(Scope &)> &expression<T>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_INSTANTIATE_EXPRESSION_RULE, (), GOLOGPP_VALUE_TYPES);


}
}