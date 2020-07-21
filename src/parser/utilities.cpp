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
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/fusion/at.hpp>

#include <model/formula.h>
#include <model/arithmetic.h>
#include <model/platform/clock_formula.h>
#include <parser/platform/constraint.h>

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
		omit[qi::string("\"")]
		> qi::lexeme [*(char_ - '"')]
		> omit[qi::string("\"")]
		, "raw_string_literal"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}




gologpp_skipper::gologpp_skipper()
: gologpp_skipper::base_type(skip, "skipper")
{
	skip = spc | comment_oneline | comment_multiline | preprocessor;

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

	preprocessor = omit [
		qi::string("#include") > *(char_ - eol)
	];
	preprocessor.name("preprocessor");
}


string get_error_context(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end
) {
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
	mark += "^ ";

	iterator::base_type l_end = errpos.base();
	while (l_end < end.base() && *l_end != '\n' && *l_end != '\r')
		++l_end;

	string line(l_start, l_end);

	std::stringstream rv;
	rv << line << std::endl << mark;
	return rv.str();
}



void handle_error(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end,
	const boost::spirit::info &expected
) {
	std::cout << "Syntax error at line " << get_line(errpos) << ":" << std::endl
		<< get_error_context(begin, errpos, end)
		<< "Expected: " << expected << std::endl;
}



template<class OperationT>
OperationT *parse_op_precedence_(op_list<OperationT> vec, Expression *last) {
	if (vec.size() == 1)
		return new OperationT(
			at_c<0>(vec.front()),
			at_c<1>(vec.front()),
			last
		);
	else {
		typename OperationT::Operator op = at_c<1>(vec.front());
		OperationT *rhs = parse_op_precedence_<OperationT>(
			op_list<OperationT>(++vec.begin(), vec.end()),
			last
		);
		if (precedence(rhs->op()) > precedence(op))
			return new OperationT(
				at_c<0>(vec.front()),
				op,
				rhs
			);
		else {
			// TODO: This is a memory leak, but what the heck this parser is full of them, anyways
			// and it's not the worst one, either.
			typename OperationT::Operator op = at_c<1>(vec.back());
			OperationT *lhs = parse_op_precedence_<OperationT>(
				op_list<OperationT>(vec.begin(), --vec.end()),
				at_c<0>(vec.back())
			);
			return new OperationT(
				lhs,
				op,
				last
			);
		}
	}
}


template<class OperationT>
OperationT *parse_op_precedence(
	vector<fusion_wtf_vector<Expression *, typename OperationT::Operator>> vec,
	Expression *rhs
) {
	return parse_op_precedence_<OperationT>(
		op_list<OperationT>(vec.begin(), vec.end()),
		rhs
	);
}

template
BooleanOperation *parse_op_precedence(
	vector<fusion_wtf_vector<Expression *, BooleanOperation::Operator>> vec,
	Expression *rhs
);

template
ArithmeticOperation *parse_op_precedence(
	vector<fusion_wtf_vector<Expression *, ArithmeticOperation::Operator>> vec,
	Expression *rhs
);

template
platform::BooleanClockOperation *parse_op_precedence(
	vector<fusion_wtf_vector<Expression *, platform::BooleanClockOperation::Operator>> vec,
	Expression *rhs
);


template
helper::BinaryOpIntermediate *parse_op_precedence(
	vector<fusion_wtf_vector<Expression *, helper::BinaryOpIntermediate::Operator>> vec,
	Expression *rhs
);



template<>
string debug_name<platform::State>()
{ return "state"; }

template<>
string debug_name<platform::Clock>()
{ return "clock"; }

template<>
string debug_name<platform::Component>()
{ return "component"; }

template<>
string debug_name<Action>()
{ return "action"; }


rule<void ()> conditional_comma(const TypeList &tl)
{
	static rule<void()> comma { ',' };
	static rule<void()> nothing { eps };
	return tl.empty() ? nothing : comma;
}


std::function<const Type &(TypeList &)> pop_front {
	[] (TypeList &tl) -> const Type & {
		const Type &rv = tl.front();
		tl.pop_front();
		return rv;
	}
};



} // namespace parser
} // namespace gologpp

