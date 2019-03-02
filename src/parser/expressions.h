#ifndef GOLOGPP_PARSER_EXPRESSIONS_H_
#define GOLOGPP_PARSER_EXPRESSIONS_H_

#include <model/expressions.h>
#include "utilities.h"

namespace gologpp {
namespace parser {


/****************************
* General expressions:
* Just an undefined template
****************************/

template<class ExprT>
struct ExpressionParser;

#define GOLOGPP_PARSER_SPECIALIZE_EXPRESSION_PARSER(r, data, T) \
template<> struct ExpressionParser<T>;

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_SPECIALIZE_EXPRESSION_PARSER, (), GOLOGPP_VALUE_TYPES)


template<class> struct ReferenceParser;
template<class> struct FieldAccessParser;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EXPRESSIONS_H_

