#include "utilities.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_not_predicate.hpp>
#include <boost/spirit/include/qi_sequence.hpp>

#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_char_.hpp>

#include <iostream>

namespace gologpp {
namespace parser {


rule<string()> &r_name() {
	static rule<string()> rv {
		qi::lexeme [
			qi::alpha >> *(qi::alnum | qi::char_('_'))
		], "name"
	};
	return rv;
};



gologpp_skipper::gologpp_skipper()
: gologpp_skipper::base_type(skip, "skipper")
{
	skip = spc | comment_oneline | comment_multiline;

	spc = space;
	spc.name("space");

	comment_multiline = omit [
		lit("/*") > *((!lit("*/")) > (char_ | eol)) > "*/"
	];
	comment_multiline.name("comment_multiline");

	comment_oneline = omit [
		lit("//") > *char_
	];
	comment_oneline.name("comment_oneline");

	BOOST_SPIRIT_DEBUG_NODES((skip)(spc)(comment_oneline)(comment_multiline))
}



void handle_error(const iterator &begin, const iterator &errpos, const iterator &end, const boost::spirit::info &expected) {
	iterator::base_type l_start;
	string mark;
	for (
		l_start = errpos.base() - 1;
		*l_start != '\n' && *l_start != '\r' && l_start >= begin.base();
		--l_start
	) {
		if (*l_start == '\t')
			mark = '\t' + mark;
		else
			mark = " " + mark;
	}
	mark += '^';

	iterator::base_type l_end = errpos.base();
	while (l_end < end.base() && *l_end != '\n' && *l_end != '\r')
		++l_end;

	string line(l_start, l_end);

	std::cout << "Syntax error at line " << get_line(errpos) << ":" << std::endl
		<< line << std::endl
		<< mark << std::endl
		<< "Expected: " << expected << std::endl;
}


} // namespace parser
} // namespace gologpp

