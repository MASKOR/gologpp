/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "utilities.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_not_predicate.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_as_string.hpp>

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


rule<string()> &raw_string_literal() {
	static rule<string()> rv {
		qi::as_string [ qi::lexeme [
			lit('"') > *(char_ - '"') > lit('"')
		] ]
		, "raw_string_literal"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}




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
		lit("//") > *(char_ - eol)
	];
	comment_oneline.name("comment_oneline");
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

