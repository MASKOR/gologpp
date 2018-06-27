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
#include <boost/phoenix/phoenix.hpp>
#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <memory>

#include <model/gologpp.h>
#include <model/effect_axiom.h>
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <model/reference.h>

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

template<class T>
static inline auto _r(T &&x)
{ return boost::phoenix::ref(std::forward<T>(x)); }



static rule<string()> r_name = qi::lexeme [
	qi::alpha >> *(qi::alnum | qi::char_('_'))
];

static rule<BooleanVariable *(Scope &)> bool_var = '?' > r_name [
	_val = bind(&Scope::variable_raw<BooleanExpression>, _r1, _1)
];

static rule<NumericVariable *(Scope &)> num_var = '%' > r_name [
	_val = bind(&Scope::variable_raw<NumericExpression>, _r1, _1)
];

static rule<BooleanConstant *()> bool_constant = (
	qi::string("true") | qi::string("false")
) [
	_val = new_<BooleanConstant>(_1)
];

static rule<NumericConstant *()> num_constant = float_ [ _val = new_<NumericConstant>(_1) ];
static rule<AbstractConstant *()> constant    = bool_constant | num_constant;
static rule<AbstractVariable *(Scope &)> var  = bool_var(_r1) | num_var(_r1);
static rule<shared_ptr<AbstractVariable>(Scope &)> var_shared = var(_r1) [ _val = construct<shared_ptr<AbstractVariable>>(_1) ];

static rule<Expression *(Scope &)> atom = bool_var(_r1)
	| attr_cast<Expression *>(num_var(_r1))
	| attr_cast<Expression *>(bool_constant)
	| attr_cast<Expression *>(bool_constant);


template<class ExpressionT>
struct UnboundReferenceParser : grammar<Reference<ExpressionT> *(Scope &)> {
	UnboundReferenceParser() : UnboundReferenceParser::base_type(pred_ref)
	{
		pred_ref = (r_name >> "(" >> (
			attr_cast<Expression *>(pred_ref(_r1))
			| attr_cast<Expression *>(bool_var(_r1))
			| attr_cast<Expression *>(bool_constant)
		) %  "," >> ")"
		) [
			_val = new_<Reference<ExpressionT>>(_1, _r1, _2)
		];
	}

	rule<Reference<ExpressionT> *(Scope &)> pred_ref;
};



struct BooleanExpressionParser : grammar<BooleanExpression *(Scope &)> {
	BooleanExpressionParser() : BooleanExpressionParser::base_type(expression)
	{
		expression = formula(_r1) | atom(_r1);

		atom = bool_constant | bool_var(_r1);
		formula = conjunction(_r1) | disjunction(_r1) | negation(_r1) | brace(_r1);

		/* Recursive alternatives are a little dumb in boost::spirit since the | operator immediately
		 * commits to the leftmost match.
		 * Cf. https://www.boost.org/doc/libs/1_67_0/libs/spirit/doc/html/spirit/qi/reference/operator/alternative.html
		 * That's why we have to do this weird switching of alternation ordering between LHS and RHS. */
		conjunction = (
			(atom(_r1) >> "&&") > expression(_r1)
		) [
			_val = new_<Conjunction>(
				construct<unique_ptr<BooleanExpression>>(_1),
				construct<unique_ptr<BooleanExpression>>(_2),
				_r1
			)
		];

		disjunction = (
			(atom(_r1) >> "||") > expression(_r1)
		) [
			_val = new_<Disjunction>(
				construct<unique_ptr<BooleanExpression>>(_1),
				construct<unique_ptr<BooleanExpression>>(_2),
				_r1
			)
		];

		negation = '!' > expression(_r1) [
			_val = new_<Negation>(construct<unique_ptr<BooleanExpression>>(_1), _r1)
		];

		brace = '(' > expression(_r1) > ')';

	}

	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> atom;
	rule<BooleanExpression *(Scope &)> formula;
	rule<BooleanExpression *(Scope &)> conjunction;
	rule<BooleanExpression *(Scope &)> disjunction;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
};


struct NumericExpressionParser : grammar<NumericExpression *(Scope &)> {
	NumericExpressionParser() : NumericExpressionParser::base_type(expression)
	{
		expression = atom(_r1) | operation(_r1);
		atom = num_constant | num_var(_r1);
		brace = '(' >> expression(_r1) >> ')';

		operation = (atom(_r1) >> arith_operator >> expression(_r1)) [
			_val = new_<ArithmeticOperation>(_1, _2, _3)
		];
		arith_operator =
				qi::string("+") [ _val = val(ArithmeticOperation::ADDITION) ]
				| qi::string("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
				| qi::string("/") [ _val = val(ArithmeticOperation::DIVISION) ]
				| qi::string("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
				| qi::string("**") [ _val = val(ArithmeticOperation::POWER) ]
				| qi::string("%") [ _val = val(ArithmeticOperation::MODULO) ]
		;
	}

	rule<NumericExpression *(Scope &)> expression;
	rule<NumericExpression *(Scope &)> atom;
	rule<NumericExpression *(Scope &)> operation;
	rule<NumericExpression *(Scope &)> brace;
	rule<ArithmeticOperation::Operator()> arith_operator;
};



struct StatementParser : grammar<Statement *(Scope &)> {
	StatementParser() : StatementParser::base_type(statement)
	{
		statement = block(_r1) | choose(_r1) | conditional(_r1) | bool_assignment(_r1)
			| numeric_assignment(_r1) | pick(_r1) | search(_r1) | test(_r1) | r_while(_r1)
			| boolean_return(_r1) | numeric_return(_r1) | procedure_call(_r1);

		block = ('{' >> (statement(_r1) % ';') >> '}') [
			_val = new_<Block>(_1, _r1)
		];

		choose = (l("choose") >> '{' >> (statement(_r1) % ',') >> '}') [
			_val = new_<Choose>(_1, _r1)
		];

		conditional = (l("if") >> '(' >> BooleanExpressionParser()(_r1) >> ')'
			>> statement(_r1) >> "else" >> statement(_r1)
		) [
			_val = new_<Conditional>(
				construct<unique_ptr<BooleanExpression>>(_1),
				construct<unique_ptr<Statement>>(_2),
				construct<unique_ptr<Statement>>(_3),
				_r1
			)
		];

		bool_assignment = (UnboundReferenceParser<BooleanFluent>()(_r1) >> '=' >> BooleanExpressionParser()(_r1)) [
			_val = new_<Assignment<BooleanExpression>>(_1, _2, _r1)
		];

		numeric_assignment = (UnboundReferenceParser<NumericFluent>()(_r1) >> '=' >> NumericExpressionParser()(_r1)) [
			_val = new_<Assignment<NumericExpression>>(_1, _2, _r1)
		];

		pick = (l("pick") >> '(' >> var_shared(_r1) >> ')' >> statement(_r1)) [
			_val = new_<Pick>(_1, _2, _r1)
		];

		search = (l("search") >> statement(_r1)) [
			_val = new_<Search>(_1, _r1)
		];

		test = (l("test") >> '(' >> BooleanExpressionParser()(_r1) >> ')') [
			_val = new_<Test>(_1, _r1)
		];

		r_while = (l("while") >> '(' >> BooleanExpressionParser()(_r1) >> ')' >> statement(_r1)) [
			_val = new_<While>(_1, _2, _r1)
		];

		boolean_return = (l("return") >> BooleanExpressionParser()(_r1)) [
			_val = new_<Return<BooleanExpression>>(_1, _r1)
		];

		numeric_return = (l("return") >> NumericExpressionParser()(_r1)) [
			_val = new_<Return<NumericExpression>>(_1, _r1)
		];
	}

	rule<Statement *(Scope &)> statement;
	rule<Block *(Scope &)> block;
	rule<Choose *(Scope &)> choose;
	rule<Conditional *(Scope &)> conditional;
	rule<Assignment<BooleanExpression> *(Scope &)> bool_assignment;
	rule<Assignment<NumericExpression> *(Scope &)> numeric_assignment;
	rule<Pick *(Scope &)> pick;
	rule<Search *(Scope &)> search;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Return<BooleanExpression> *(Scope &)> boolean_return;
	rule<Return<NumericExpression> *(Scope &)> numeric_return;
	UnboundReferenceParser<Procedure> procedure_call;
};




struct EffectParser : grammar<AbstractEffectAxiom *(shared_ptr<Action> &, Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect)
	{
		effect = boolean_effect(_r1, _r2) | numeric_effect(_r1, _r2);

		boolean_effect = (
			BooleanExpressionParser()(_r2) >> "->"
			>> UnboundReferenceParser<BooleanFluent>()(_r2) >> '=' >> BooleanExpressionParser()(_r2)
		) [
			_val = new_<EffectAxiom<BooleanExpression>>(_r1, _1, _2, _3)
		];

		numeric_effect = (
			BooleanExpressionParser()(_r2) >> "->"
			>> UnboundReferenceParser<NumericFluent>()(_r2) >> '=' >> NumericExpressionParser()(_r2)
		) [
			_val = new_<EffectAxiom<NumericExpression>>(_r1, _1, _2, _3)
		];
	}

	rule<AbstractEffectAxiom *(shared_ptr<Action> &, Scope &)> effect;
	rule<EffectAxiom<BooleanExpression> *(shared_ptr<Action> &, Scope &)> boolean_effect;
	rule<EffectAxiom<NumericExpression> *(shared_ptr<Action> &, Scope &)> numeric_effect;
};



struct ActionParser : grammar<shared_ptr<Action>()> {
	ActionParser()
	: ActionParser::base_type(action)
	{
		action =
			(
				("action" >> r_name >> '(') [ _r(scope) = new_<Scope>(nullptr) ]
				>> *var_shared(_r(*scope)) >> ')'
			) [
				_val = construct<shared_ptr<Action>>(
					new_<Action>(_r(scope), _1, _2)
				)
			]
			>> '{'
			>> "precondition:" >> formula(_r(*scope)) [
				phoenix::bind(
					&Action::set_precondition,
					_val,
					_1
				)
			] //*/
			>> "effect:" >> (EffectParser()(_val, _r(*scope)) [
				phoenix::bind(
					&Action::add_effect,
					_val,
					_1
				)
			] % ';')
			//>> "signal:" //*/
			>> '}' //*/
		;
	}

	BooleanExpressionParser formula;
	Scope *scope;
	rule<shared_ptr<Action>()> action;
};



struct FluentParser : grammar<AbstractFluent *()> {
	FluentParser()
	: FluentParser::base_type(fluent)
	{
		fluent = (
			(l("?") >> "fluent" >> r_name >> '(') [ _r(scope) = new_<Scope>(nullptr) ]
			>> *var_shared(_r(*scope)) >> ')' >> '=' >> bool_constant
		) [
			_val = new_<BooleanFluent>(_r(scope), _1, _2, construct<unique_ptr<BooleanExpression>>(_3))
		]
		| (
			(l("%") >> "fluent" >> r_name >> '(') [ _r(scope) = new_<Scope>(nullptr) ]
			>> *var_shared(_r(*scope)) >> ')' >> '=' >> num_constant
		) [
			_val = new_<NumericFluent>(_r(scope), _1, _2, construct<unique_ptr<NumericExpression>>(_3))
		];
	}

	rule<AbstractFluent *()> fluent;
	Scope *scope;
};





} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_GRAMMAR_H_
