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
inline auto
type_mark();

template<>
inline auto
type_mark<BooleanExpression>()
{ return boost::spirit::qi::lit('?'); }

template<>
inline auto
type_mark<NumericExpression>()
{ return boost::spirit::qi::lit('%'); }

template<>
inline auto
type_mark<SymbolicExpression>()
{ return boost::spirit::qi::lit("§"); }

template<>
inline auto
type_mark<StringExpression>()
{ return boost::spirit::qi::lit('$'); }

template<>
inline auto
type_mark<CompoundExpression>()
{ return boost::spirit::qi::lit('@'); }



template<class ExpressionT>
struct type_descr;

template<>
struct type_descr<BooleanExpression> {
	inline string operator () ()
	{ return "boolean"; }
};

template<>
struct type_descr<NumericExpression> {
	inline string operator () ()
	{ return "numeric"; }
};

template<>
struct type_descr<SymbolicExpression> {
	inline string operator () ()
	{ return "symbolic"; }
};

template<>
struct type_descr<StringExpression> {
	inline string operator () ()
	{ return "string"; }
};

template<>
struct type_descr<VoidExpression> {
	inline string operator () ()
	{ return "void"; }
};

template<>
struct type_descr<CompoundExpression> {
	inline string operator () ()
	{ return "compound"; }
};

template<class ExprT>
struct type_descr<Function<ExprT>> {
	inline string operator () ()
	{ return type_descr<ExprT>()() + "_function"; }
};

template<class ExprT>
struct type_descr<Fluent<ExprT>> {
	inline string operator () ()
	{ return type_descr<ExprT>()() + "_fluent"; }
};

template<>
struct type_descr<ExogAction> {
	inline string operator () ()
	{ return "exog_action"; }
};

template<>
struct type_descr<Action> {
	inline string operator () ()
	{ return "action"; }
};



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
