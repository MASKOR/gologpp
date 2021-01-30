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

#ifndef GOLOGPP_PARSER_UTILITIES_H_
#define GOLOGPP_PARSER_UTILITIES_H_

#include <model/expressions.h>
#include <model/error.h>

#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>

#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/phoenix/statement/sequence.hpp>


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


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


using iterator = boost::spirit::line_pos_iterator<string::const_iterator>;



struct gologpp_skipper : public qi::grammar<iterator> {
	gologpp_skipper();
	
	qi::rule<iterator> skip;
	qi::rule<iterator> spc;
	qi::rule<iterator> comment_multiline;
	qi::rule<iterator> comment_oneline;
	qi::rule<iterator> preprocessor;
};


template<typename... SignatureTs>
using rule = boost::spirit::qi::rule<iterator, gologpp_skipper, SignatureTs...>;

template<typename... SignatureTs>
using grammar = boost::spirit::qi::grammar<iterator, gologpp_skipper, SignatureTs...>;



rule<string()> &r_name();

rule<string()> &raw_string_literal();


string get_error_context(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end
);

void throw_syntax_error(
	const iterator &begin,
	const iterator &errpos,
	const iterator &end,
	const boost::spirit::info &expected
);



template<class OperationT, class ExprT>
using op_list = std::list<fusion_wtf_vector<ExprT *, typename OperationT::Operator>>;

/*
 * Behold the braindeadness of PEGs: Operator precedence is impossible to implement
 * because left recursion is forbidden. Insert facepalm here.
 *
 * So first, we have to parse sequences of unary expressions and operators into an unstructured
 * list. Then shove it all into this "after-parser" to build a structured BooleanOperation
 * that properly reflects operator precedence.
 */
template<class OperationT, class ExprT = Expression>
OperationT *parse_op_precedence(
	vector<fusion_wtf_vector<ExprT *, typename OperationT::Operator>> vec,
	ExprT *rhs
);


template<class PlatformT>
string debug_name();


rule<void()> conditional_comma(const TypeList &tl);


// These are basically workarounds for cases where the normal boost::phoenix return type
// deduction doesn't work due to weird interactions with the STL
extern std::function<const Type &(TypeList &)> type_list_pop_front;

extern std::function<TypeList::size_type(const TypeList &)> type_list_size;


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

