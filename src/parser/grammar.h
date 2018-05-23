#ifndef GOLOGPP_GRAMMAR_H_
#define GOLOGPP_GRAMMAR_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/scope/lambda.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/core/value.hpp>
#include <boost/phoenix/object/static_cast.hpp>

#include <string>
#include <vector>
#include <memory>

#include <model/gologpp.h>
#include <model/effect_axiom.h>
#include <model/action.h>

namespace gologpp {
namespace parser {


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace attr = boost::spirit;


using namespace boost::spirit;
using namespace boost::spirit::qi::labels;
using namespace boost::phoenix;


template<typename ResultT>
using rule = boost::spirit::qi::rule<string::const_iterator, ResultT, ascii::space_type>;

template<typename ResultT>
using grammar = boost::spirit::qi::grammar<string::const_iterator, ResultT, ascii::space_type>;

template<class T>
static inline auto l(T x)
{ return qi::lit(x); }


static rule<string()> r_name = qi::lexeme [
	qi::alpha >> *(qi::alnum | qi::char_('_'))
];

static rule<BooleanVariable *(Scope &)> bool_var = qi::lit('?') >> r_name [
	_val = bind(&Scope::variable_raw<BooleanExpression>, _r1, _1)
];

static rule<NumericVariable *(Scope &)> num_var = qi::lit('%') >> r_name [
	_val = bind(&Scope::variable_raw<NumericExpression>, _r1, _1)
];

static rule<BooleanConstant *()> bool_constant = (
	qi::string("true") | qi::string("false")
) [
	_val = new_<BooleanConstant>(_1)
];



struct BooleanExpressionParser : grammar<BooleanExpression *(Scope &)> {
	BooleanExpressionParser() : BooleanExpressionParser::base_type(expression)
	{
		expression = atom(_r1) | formula(_r1);

		atom = bool_constant | bool_var(_r1);
		formula = conjunction(_r1) | disjunction(_r1) | negation(_r1) | brace(_r1);

		/* Recursive alternatives are a little dumb in boost::spirit since the | operator immediately
		 * commits to the leftmost match.
		 * Cf. https://www.boost.org/doc/libs/1_67_0/libs/spirit/doc/html/spirit/qi/reference/operator/alternative.html
		 * That's why we have to do this weird switching of alternation ordering between LHS and RHS. */
		conjunction = (
			(atom(_r1) | formula(_r1))
			>> qi::lit("&&")
			>> (formula(_r1) | (atom(_r1)))
		) [
			_val = new_<Conjunction>(
				construct<unique_ptr<BooleanExpression>>(_1),
				construct<unique_ptr<BooleanExpression>>(_2),
				_r1
			)
		];

		disjunction = (
			(atom(_r1) | formula(_r1))
			>> qi::lit("||")
			>> (formula(_r1) | atom(_r1))
		) [
			_val = new_<Disjunction>(
				construct<unique_ptr<BooleanExpression>>(_1),
				construct<unique_ptr<BooleanExpression>>(_2),
				_r1
			)
		];
	}

	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> atom;
	rule<BooleanExpression *(Scope &)> formula;
	rule<BooleanExpression *(Scope &)> conjunction;
	rule<BooleanExpression *(Scope &)> disjunction;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
};



struct ActionParser : grammar<shared_ptr<Action>()> {
	ActionParser()
	: ActionParser::base_type(action)
	{
		action =
			(
				l("action") >> r_name >> l('(') [ ref(scope) = new_<Scope>(nullptr) ]
				>> *var_shared(ref(*scope)) >> l(')')
			) [
				_val = construct<shared_ptr<Action>>(
					new_<Action>(ref(scope), _1, _2)
				)
			]
			>> l("{") >> l("precondition:") >> formula(ref(*scope)) [
				phoenix::bind(
					&Action::set_precondition_ptr,
					_val,
					ref(_1)
				)
			] //*/
			>> l("effect:")
			>> l("signal:")
			>> l('}') //*/
		;
	}

	FormulaParser p_formula;
	Scope *scope;
	rule<shared_ptr<Action>()> r_action;
	rule<BooleanExpression *(Scope &)> r_precond;
};





} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_GRAMMAR_H_
