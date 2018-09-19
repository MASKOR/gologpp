#ifndef GOLOGPP_PARSER_TYPES_H_
#define GOLOGPP_PARSER_TYPES_H_

#include <model/gologpp.h>

#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>

namespace gologpp {
namespace parser {


// The following template specializations need to be inlined
// to avoid multiple definition errors since this file is
// included in multiple objects


template<class ExpressionT>
auto type_mark();

template<>
inline auto type_mark<BooleanExpression>()
{ return boost::spirit::qi::lit('?'); }

template<>
inline auto type_mark<NumericExpression>()
{ return boost::spirit::qi::lit('%'); }



template<class ExpressionT>
string type_descr();

template<>
inline string type_descr<BooleanExpression>()
{ return "boolean"; }

template<>
inline string type_descr<NumericExpression>()
{ return "numeric"; }





} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_TYPES_H_
