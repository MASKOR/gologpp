#ifndef GOLOGPP_PARSER_UTILITIES_H_
#define GOLOGPP_PARSER_UTILITIES_H_

#include <model/expressions.h>

#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>


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


void handle_error(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end,
	const boost::spirit::info &expected
);



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_UTILITIES_H_

