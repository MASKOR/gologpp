#ifndef GOLOGPP_PARSER_TYPES_H_
#define GOLOGPP_PARSER_TYPES_H_

#include <model/gologpp.h>
#include <model/types.h>

#include "utilities.h"

#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

namespace gologpp {
namespace parser {


// The following template specializations need to be inlined
// to avoid multiple definition errors since this file is
// included in multiple objects


template<class ExpressionT>
inline rule<void ()>
type_mark();

template<>
inline rule<void ()>
type_mark<BooleanExpression>()
{ return boost::spirit::qi::lit('?'); }

template<>
inline rule<void ()>
type_mark<NumericExpression>()
{ return boost::spirit::qi::lit('%'); }

template<>
inline rule<void ()>
type_mark<SymbolicExpression>()
{ return boost::spirit::qi::lit("§"); }

template<>
inline rule<void ()>
type_mark<StringExpression>()
{ return boost::spirit::qi::lit('$'); }

template<>
inline rule<void ()>
type_mark<CompoundExpression>()
{ return boost::spirit::qi::lit('@'); }



template<class ExpressionT>
string type_descr();

template<>
inline string type_descr<BooleanExpression>()
{ return "boolean"; }

template<>
inline string type_descr<NumericExpression>()
{ return "numeric"; }

template<>
inline string type_descr<SymbolicExpression>()
{ return "symbolic"; }

template<>
inline string type_descr<StringExpression>()
{ return "string"; }

template<>
inline string type_descr<VoidExpression>()
{ return "void"; }

template<>
inline string type_descr<CompoundExpression>()
{ return "compound"; }



struct CompoundTypeParser : public grammar<CompoundType *(Scope &)> {
	CompoundTypeParser();

	rule<CompoundType *(Scope &)> type_definition;
};



template<class ExprT>
rule<string()> &
type_specifier()
{
	// Default (simple types): nothing to parse, just return the type's defined name
	static rule<string()> type_specifier {
		eps [
			_val = phoenix::bind(&ExprT::type_t::static_name)
		]
	};
	return type_specifier;
}


template<>
rule<string()> &
type_specifier<CompoundExpression>();





} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_TYPES_H_
