#ifndef GOLOGPP_PARSER_EXPRESSIONS_H_
#define GOLOGPP_PARSER_EXPRESSIONS_H_

#include <model/expressions.h>

namespace gologpp {
namespace parser {


/****************************
* General expressions:
* Just an undefined template
****************************/

template<class ExprT>
struct ExpressionParser;

typedef ExpressionParser<BooleanExpression> BooleanExpressionParser;
typedef ExpressionParser<NumericExpression> NumericExpressionParser;

} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EXPRESSIONS_H_

