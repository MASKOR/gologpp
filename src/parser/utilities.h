#ifndef GOLOGPP_PARSER_UTILITIES_H_
#define GOLOGPP_PARSER_UTILITIES_H_

#include <model/expressions.h>

#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>

#include <boost/preprocessor/seq/for_each.hpp>


namespace gologpp {
namespace parser {


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace attr = boost::spirit;


using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::qi::labels;
using namespace boost::phoenix;


using Typename = string;


/******************
* Little helpers
******************/

using iterator = boost::spirit::line_pos_iterator<string::const_iterator>;



struct gologpp_skipper : public qi::grammar<iterator> {
	gologpp_skipper();
	
	qi::rule<iterator> skip;
	qi::rule<iterator> spc;
	qi::rule<iterator> comment_multiline;
	qi::rule<iterator> comment_oneline;
};


template<typename... SignatureTs>
using rule = boost::spirit::qi::rule<iterator, gologpp_skipper, SignatureTs...>;

template<typename... SignatureTs>
using grammar = boost::spirit::qi::grammar<iterator, gologpp_skipper, SignatureTs...>;



rule<string()> &r_name();

rule<string()> &raw_string_literal();


void handle_error(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end,
	const boost::spirit::info &expected
);


#ifdef GOLOGPP_DEBUG_PARSER
#define GOLOGPP_DEBUG_NODE(n) debug(n);
#define GOLOGPP_DEBUG_NODE_A(_r, _data, node) GOLOGPP_DEBUG_NODE(node)
#define GOLOGPP_DEBUG_NODES(n) BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEBUG_NODE_A, (), n)
#else
#define GOLOGPP_DEBUG_NODE(n)
#define GOLOGPP_DEBUG_NODES(n)
#endif


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_UTILITIES_H_

