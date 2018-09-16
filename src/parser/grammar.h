#ifndef GOLOGPP_GRAMMAR_H_
#define GOLOGPP_GRAMMAR_H_


#ifdef DEBUG_PARSER
#define BOOST_SPIRIT_DEBUG
#endif


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/stl/container.hpp>

#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <model/gologpp.h>
#include <model/effect_axiom.h>
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <model/reference.h>
#include <model/atoms.h>
#include <model/global.h>
#include <model/arithmetic.h>
#include <model/formula.h>

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

template<class T>
static inline auto l(T x)
{ return qi::lit(x); }

template<class ExpressionT>
static auto type_mark();

template<>
auto type_mark<BooleanExpression>()
{ return qi::lit('?'); }

template<>
auto type_mark<NumericExpression>()
{ return qi::lit('%'); }


template<class ExpressionT>
static string type_descr();

template<>
string type_descr<BooleanExpression>()
{ return "boolean"; }

template<>
string type_descr<NumericExpression>()
{ return "numeric"; }


using iterator = boost::spirit::line_pos_iterator<string::const_iterator>;


static qi::rule<iterator> comment_multiline {
	omit [ l("/*") > *((!l("*/")) > (char_ | eol)) > "*/" ],
	"comment_multiline"
};

static qi::rule<iterator> comment_oneline {
	omit [ l("//") > *char_ ],
	"comment_oneline"
};


struct gologpp_skipper : public qi::grammar<iterator> {
	gologpp_skipper()
	: gologpp_skipper::base_type(skip, "skipper")
	{
		skip = spc | comment_oneline | comment_multiline;
		spc = space;
		spc.name("space");
		BOOST_SPIRIT_DEBUG_NODES((skip)(spc)(comment_oneline)(comment_multiline))
	}
	qi::rule<iterator> skip;
	qi::rule<iterator> spc;
};


template<typename... SignatureTs>
using rule = boost::spirit::qi::rule<iterator, gologpp_skipper, SignatureTs...>;

template<typename... SignatureTs>
using grammar = boost::spirit::qi::grammar<iterator, gologpp_skipper, SignatureTs...>;



static rule<string()> r_name(qi::lexeme [
	qi::alpha >> *(qi::alnum | qi::char_('_'))
], "name");



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


/******************
 * Variables
 ******************/

template<class ExpressionT>
struct VariableParser : grammar<shared_ptr<Variable<ExpressionT>>(Scope &)> {
	VariableParser() : VariableParser::base_type(variable, type_descr<ExpressionT>() + "_variable") {
		variable = type_mark<ExpressionT>() >> r_name [
			_val = phoenix::bind(
				&Scope::get_var<ExpressionT>, _r1, _1
			)
		];
	}

	rule<shared_ptr<Variable<ExpressionT>>(Scope &)> variable;
};

static VariableParser<BooleanExpression> bool_var;
static VariableParser<NumericExpression> num_var;



static rule<shared_ptr<AbstractVariable> (Scope &)> abstract_var(
	bool_var(_r1) [ _val = phoenix::bind(
		&std::dynamic_pointer_cast<AbstractVariable, BooleanVariable>,
		_1
	) ]
	| num_var(_r1) [ _val = phoenix::bind(
		&std::dynamic_pointer_cast<AbstractVariable, NumericVariable>,
		_1
	) ]
, "any_variable");



/******************
* Constants
******************/

static rule<Constant<BooleanExpression> *> bool_constant( (
	qi::string("true") | qi::string("false")
) [
	_val = new_<Constant<BooleanExpression>>(_1)
], "boolean_constant");


static rule<Constant<NumericExpression> *> num_constant((int_ | float_) [
	_val = new_<Constant<NumericExpression>>(_1)
], "numeric_constant");


template<class ExprT>
static rule<Constant<ExprT> *> &constant();

template<>
rule<Constant<NumericExpression> *> &constant<NumericExpression>()
{ return num_constant; }

template<>
rule<Constant<BooleanExpression> *> &constant<BooleanExpression>()
{ return bool_constant; }


static rule<AbstractConstant *> abstract_constant(
	bool_constant | num_constant, "any_constant"
);



/******************
* General atoms
******************/

static rule<Expression *(Scope &)> atom(
	bool_var(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ]
	| num_var(_r1) [ _val = new_<Reference<NumericVariable>>(_1) ]
	| bool_constant [ _val = _1 ]
	| num_constant [ _val = _1 ]
, "any_atom");



/******************
* References
******************/

template<class GologT>
struct ReferenceParser : grammar<Reference<GologT> *(Scope &)> {
	ReferenceParser() : ReferenceParser::base_type(pred_ref, string("reference_to_") + typeid(GologT).name())
	{
		pred_ref = ((r_name > "(" > -(
			(atom(_r1) | any_pred_ref(_r1)) %  ","
		) ) > ")") [
			_val = new_<Reference<GologT>>(_1, _2),
			if_(!phoenix::bind(&Reference<GologT>::consistent, *_val)) [
				_pass = false
			]
		];
		on_error<fail>(pred_ref, delete_(_val));

		any_pred_ref = ((r_name > "(" > -(
			(atom(_r1) | any_pred_ref(_r1)) % ","
		) ) > ")") [
			_val = phoenix::bind(&ref_to_global, _1, _2),
			if_(!phoenix::bind(&AbstractReference::consistent, dynamic_cast_<AbstractReference &>(*_val))) [
				_pass = false
			]
		];

		BOOST_SPIRIT_DEBUG_NODES((pred_ref)(any_pred_ref));
	}

	rule<Reference<GologT> *(Scope &)> pred_ref;
	rule<Expression *(Scope &)> any_pred_ref;
};



/****************************
* General expressions:
* Just an undefined template
****************************/

template<class ExprT>
struct ExpressionParser;



/*********************
* Numeric expressions
*********************/

template<>
struct ExpressionParser<NumericExpression> : grammar<NumericExpression *(Scope &)> {
	ExpressionParser()
	: ExpressionParser::base_type(expression, "numeric_expression")
	{
		expression = binary_expr(_r1) | unary_expr(_r1);
		expression.name("numeric_expression");

		unary_expr = brace(_r1) | num_constant | num_var_ref(_r1) | num_fluent_ref(_r1) | num_function_ref(_r1);
		unary_expr.name("unary_numeric_expression");

		binary_expr = (
			(unary_expr(_r1) >> arith_operator) > expression(_r1)
		) [
			_val = new_<ArithmeticOperation>(at_c<0>(_1), at_c<1>(_1), _2)
		];
		binary_expr.name("binary_numeric_expression");

		brace = '(' >> expression(_r1) >> ')';
		brace.name("braced_numeric_expression");

		arith_operator =
			qi::string("+") [ _val = val(ArithmeticOperation::ADDITION) ]
			| qi::string("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
			| qi::string("/") [ _val = val(ArithmeticOperation::DIVISION) ]
			| qi::string("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
			| qi::string("**") [ _val = val(ArithmeticOperation::POWER) ]
			| qi::string("%") [ _val = val(ArithmeticOperation::MODULO) ]
		;
		arith_operator.name("arithmetic_operator");

		num_var_ref = num_var(_r1) [
			_val = new_<Reference<NumericVariable>>(_1)
		];
		num_var_ref.name("reference_to_numeric_variable");

		BOOST_SPIRIT_DEBUG_NODES((expression)(binary_expr)(unary_expr)
			(operation)(brace)(num_var_ref)(arith_operator));
	}

	rule<NumericExpression *(Scope &)> expression;
	rule<NumericExpression *(Scope &)> binary_expr;
	rule<NumericExpression *(Scope &)> operation;
	rule<NumericExpression *(Scope &)> brace;
	rule<NumericExpression *(Scope &)> num_var_ref;
	rule<NumericExpression *(Scope &)> unary_expr;
	rule<ArithmeticOperation::Operator()> arith_operator;
	ReferenceParser<NumericFluent> num_fluent_ref;
	ReferenceParser<NumericFunction> num_function_ref;
};

typedef ExpressionParser<NumericExpression> NumericExpressionParser;



/*********************
* Boolean expressions
*********************/

template<>
struct ExpressionParser<BooleanExpression> : grammar<BooleanExpression *(Scope &)> {
	ExpressionParser()
	: ExpressionParser::base_type(expression, "boolean_expression")
	{
		expression = binary_expr(_r1) | unary_expr(_r1);
		expression.name("boolean_expression");

		unary_expr = quantification(_r1) | negation(_r1) | bool_constant
			| bool_var_ref(_r1) | brace(_r1) | num_comparison(_r1) | bool_fluent_ref(_r1) | bool_function_ref(_r1);
		unary_expr.name("unary_boolean_expression");

		binary_expr = (
			(unary_expr(_r1) >> bool_op) > expression(_r1)
		) [
			_val = new_<BooleanOperation>(at_c<0>(_1), at_c<1>(_1), _2)
		];
		binary_expr.name("binary_boolean_expression");

		num_comparison = (
			(num_expression(_r1) >> num_cmp_op) > num_expression(_r1)
		) [
			_val = new_<Comparison>(at_c<0>(_1), at_c<1>(_1), _2)
		];
		num_comparison.name("numeric_comparison");

		quantification = ( (quantification_op > '(') [
			_a = new_<Scope>(_r1)
		] > abstract_var(*_a) > ')' > expression(*_a)) [
			_val = new_<Quantification>(_a, _1, _2, _3)
		];
		quantification.name("quantification");
		on_error<fail>(quantification, delete_(_a));

		quantification_op = qi::string("exists") [ _val = val(QuantificationOperator::EXISTS) ]
			| qi::string("forall") [ _val = val(QuantificationOperator::FORALL) ];
		quantification_op.name("quantification_operator");

		bool_op =
			qi::string("==") [ _val = val(BooleanOperator::IFF) ]
			| qi::string("!=") [ _val = val(BooleanOperator::XOR) ]
			| qi::string("&") [ _val = val(BooleanOperator::AND) ]
			| qi::string("|") [ _val = val(BooleanOperator::OR) ]
			| qi::string("->") [ _val = val(BooleanOperator::IMPLIES) ]
		;
		bool_op.name("boolean_operator");

		num_cmp_op =
			qi::string(">") [ _val = val(ComparisonOperator::GT) ]
			| qi::string(">=") [ _val = val(ComparisonOperator::GE) ]
			| qi::string("<=") [ _val = val(ComparisonOperator::LE) ]
			| qi::string("<") [ _val = val(ComparisonOperator::LT) ]
			| qi::string("==") [ _val = val(ComparisonOperator::EQ) ]
			| qi::string("!=") [ _val = val(ComparisonOperator::NEQ) ]
		;
		num_cmp_op.name("numeric_comparison_operator");

		negation = '!' > unary_expr(_r1) [
			_val = new_<Negation>(_1)
		];
		negation.name("negation");

		brace = '(' >> expression(_r1) >> ')';
		brace.name("braced_boolean_expression");

		bool_var_ref = bool_var(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ];
		bool_var_ref.name("reference_to_boolean_variable");

		BOOST_SPIRIT_DEBUG_NODES((expression)(unary_expr)(binary_expr)
			(negation)(brace)(bool_var_ref)(quantification)
			(quantification_op)(bool_op)(num_comparison)(num_cmp_op));
	}

	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> unary_expr;
	rule<BooleanExpression *(Scope &)> binary_expr;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
	rule<BooleanExpression *(Scope &)> bool_var_ref;
	rule<BooleanExpression *(Scope &), locals<Scope *>> quantification;
	rule<QuantificationOperator()> quantification_op;
	ReferenceParser<BooleanFluent> bool_fluent_ref;
	ReferenceParser<BooleanFunction> bool_function_ref;
	rule<BooleanOperator()> bool_op;
	rule<BooleanExpression *(Scope &)> num_comparison;
	NumericExpressionParser num_expression;
	rule<ComparisonOperator()> num_cmp_op;
};

typedef ExpressionParser<BooleanExpression> BooleanExpressionParser;



/******************
* Assignments
******************/

template<class LhsT>
struct AssignmentParser;


template<class ExpressionT>
struct AssignmentParser<Fluent<ExpressionT>> : grammar<Assignment<Fluent<ExpressionT>> *(Scope &)> {
	AssignmentParser()
	: AssignmentParser<Fluent<ExpressionT>>::base_type(assignment, string("assignment_to_") + type_descr<ExpressionT>() + "_fluent")
	{
		assignment = (fluent_ref(_r1) >> "=" > expression(_r1)) [
			_val = new_<Assignment<Fluent<ExpressionT>>>(_1, _2)
		];
	}

	rule<Assignment<Fluent<ExpressionT>> *(Scope &)> assignment;
	ReferenceParser<Fluent<ExpressionT>> fluent_ref;
	ExpressionParser<ExpressionT> expression;
};


template<class ExpressionT>
struct AssignmentParser<Variable<ExpressionT>> : grammar<Assignment<Variable<ExpressionT>> *(Scope &)> {
	AssignmentParser()
	: AssignmentParser<Variable<ExpressionT>>::base_type(assignment, string("assignment_to_") + type_descr<ExpressionT>() + "_variable")
	{
		assignment = (var_ref(_r1) >> "=" > expression(_r1)) [
			_val = new_<Assignment<Variable<ExpressionT>>>(_1, _2)
		];

		var_ref = var(_r1) [ _val = new_<Reference<Variable<ExpressionT>>>(_1) ];
		var_ref.name("reference_to_" + type_descr<ExpressionT>() + "_variable");
	}

	rule<Assignment<Variable<ExpressionT>> *(Scope &)> assignment;
	rule<Reference<Variable<ExpressionT>> *(Scope &)> var_ref;
	ExpressionParser<ExpressionT> expression;
	VariableParser<ExpressionT> var;
};



/******************
* Statements
******************/

struct StatementParser : grammar<Statement *(Scope &)> {
	StatementParser()
	: StatementParser::base_type(statement, "statement")
	{
		statement = compound_statement(_r1) | simple_statement(_r1);
		statement.name("statement");

		simple_statement = (test(_r1) | boolean_return(_r1) | numeric_return(_r1)
			| numeric_var_assignment(_r1) | bool_var_assignment(_r1)
			| numeric_fluent_assignment(_r1) | bool_fluent_assignment(_r1)
			| action_call(_r1) | procedure_call(_r1)) > ';';
		simple_statement.name("simple_statement");

		compound_statement = block(_r1) | choose(_r1) | conditional(_r1)
			| pick(_r1) | solve(_r1) | search(_r1) | r_while(_r1);
		compound_statement.name("compound_statement");


		block = (l('{') [
			_a = new_<Scope>(_r1)
		] > +statement(*_a) > '}') [
			_val = new_<Block>(_a, _1)
		];
		block.name("block");
		on_error<fail>(block, delete_(_a));


		choose = ((l("choose") > '{') [
			_a = new_<Scope>(_r1)
		] > +statement(*_a) > '}') [
			_val = new_<Choose>(_a, _1)
		];
		choose.name("choose");
		on_error<fail>(choose, delete_(_a));


		conditional = (l("if") > '(' > boolean_expression(_r1) > ')'
			> statement(_r1) > -("else" > statement(_r1))
		) [
			_val = new_<Conditional>(_1, _2, _3)
		];
		conditional.name("conditional");


		pick = ((l("pick") > '(') [
			_a = new_<Scope>(_r1)
		] > abstract_var(*_a) > ')' > statement(*_a)) [
			_val = new_<Pick>(_a, _1, _2)
		];
		pick.name("pick");
		on_error<fail>(pick, delete_(_a));


		search = (l("search") > statement(_r1)) [
			_val = new_<Search>(_1)
		];
		search.name("search");

		solve = (l("solve") > '('
			> numeric_expression(_r1) > ',' > reward_fn(_r1)
		> ')' > statement(_r1)) [
			_val = new_<Solve>(_1, _2, _3)
		];

		test = (l("test") > '(' > boolean_expression(_r1) > ')') [
			_val = new_<Test>(_1)
		];
		test.name("test");

		r_while = (l("while") > '(' > boolean_expression(_r1) > ')' > statement(_r1)) [
			_val = new_<While>(_1, _2)
		];
		r_while.name("while");

		boolean_return = (l("return") >> boolean_expression(_r1)) [
			_val = new_<Return<BooleanExpression>>(_1)
		];
		boolean_return.name("boolean_return");

		numeric_return = (l("return") >> numeric_expression(_r1)) [
			_val = new_<Return<NumericExpression>>(_1)
		];
		numeric_return.name("numeric_return");

		BOOST_SPIRIT_DEBUG_NODES((statement)(simple_statement)(compound_statement)
			(block)(choose)(conditional)(pick)(search)(solve)(test)(r_while)
			(boolean_return)(numeric_return));
	}

	rule<Statement *(Scope &)> statement;
	rule<Statement *(Scope &)> simple_statement;
	rule<Statement *(Scope &)> compound_statement;
	rule<Block *(Scope &), locals<Scope *>> block;
	rule<Choose *(Scope &), locals<Scope *>> choose;
	rule<Conditional *(Scope &)> conditional;
	AssignmentParser<BooleanFluent> bool_fluent_assignment;
	AssignmentParser<NumericFluent> numeric_fluent_assignment;
	AssignmentParser<BooleanVariable> bool_var_assignment;
	AssignmentParser<NumericVariable> numeric_var_assignment;
	rule<Pick *(Scope &), locals<Scope *>> pick;
	rule<Search *(Scope &)> search;
	rule<Solve *(Scope &)> solve;
	ReferenceParser<NumericFunction> reward_fn;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Return<BooleanExpression> *(Scope &)> boolean_return;
	rule<Return<NumericExpression> *(Scope &)> numeric_return;
	ReferenceParser<Procedure> procedure_call;
	ReferenceParser<Action> action_call;
	BooleanExpressionParser boolean_expression;
	NumericExpressionParser numeric_expression;

};



/******************
* Functions
******************/

template<class ExpressionT>
struct FunctionParser : grammar<Function<ExpressionT> *(Scope &), locals<Scope *>> {
	FunctionParser()
	: FunctionParser::base_type(function, type_descr<ExpressionT>() + "_function_definition")
	{
		function = function_forward(_r1) | function_def(_r1);
		function.name(type_descr<ExpressionT>() + "_function_definition");

		function_forward = (
			((type_mark<ExpressionT>() >> "function") >> r_name >> '(') [
				_a = new_<Scope>(_r1)
			]
			>> -(abstract_var(*_a) % ',') >> ')' >> ';'
		) [
			_val = phoenix::bind(
				&Scope::declare_global<Function<ExpressionT>>,
				phoenix::bind(&global_scope),
				_a, _1, _2
			)
		];
		function_forward.name(type_descr<ExpressionT>() + "_function_forward_declaration");

		function_def = (
			((type_mark<ExpressionT>() >> "function") >> r_name >> '(') [
				_a = new_<Scope>(phoenix::bind(&AbstractLanguageElement::m_scope, _r1))
			]
			>> -(abstract_var(*_a) % ',') >> ')' >> statement(_r1)
		) [
			_val = phoenix::bind(
				&Scope::define_global<Function<ExpressionT>, Statement *>,
				phoenix::bind(&global_scope),
				_a, _1, _2, _3
			)
		];
		function_def.name(type_descr<ExpressionT>() + "_function_definition");


		on_error<fail>(function, delete_(_a));
		BOOST_SPIRIT_DEBUG_NODE(function);
	}

	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function_forward;
	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function_def;
	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function;
	StatementParser statement;
};



struct AbstractFunctionParser : grammar<AbstractFunction *(Scope &)> {
	AbstractFunctionParser()
	: AbstractFunctionParser::base_type(function, "any_function_definition")
	{
		function = bool_func(_r1) | num_func(_r1);
		function.name("any_function_definition");
		BOOST_SPIRIT_DEBUG_NODE(function);
	}

	rule<AbstractFunction *(Scope &)> function;
	FunctionParser<NumericExpression> num_func;
	FunctionParser<BooleanExpression> bool_func;
};



/******************
* Effect axioms
******************/

template<class ExprT>
struct EffectParser : grammar<EffectAxiom<ExprT> *(Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect, "effect_axiom")
	{
		effect = ( eps [ _val = new_<EffectAxiom<ExprT>>() ] >> (
			-(condition(_r1) >> "->")
			>> fluent_ref(_r1)
			>> '=' >> expression(_r1)
		)) [
			phoenix::bind(&EffectAxiom<ExprT>::define, *_val, _1, _2, _3)
		];
		effect.name("boolean_effect_axiom");
		on_error<rethrow>(effect, delete_(_val));

		BOOST_SPIRIT_DEBUG_NODE(effect);
	}

	BooleanExpressionParser condition;
	ExpressionParser<ExprT> expression;
	ReferenceParser<Fluent<ExprT>> fluent_ref;
	rule<EffectAxiom<ExprT> *(Scope &)> effect;
};



/******************
* Actions
******************/

struct ActionParser : grammar<Action *(Scope &)> {
	ActionParser()
	: ActionParser::base_type(action, "action_declaration")
	{
		action = action_forward(_r1) | action_def(_r1);

		action_forward = ( (
			(("action" >> r_name >> '(') [
				_a = new_<Scope>(_r1)
			])
			>> -(abstract_var(*_a) % ',') >> ')'
		) >> l(';') ) [
			phoenix::bind(&Scope::declare_global<Action>, _r1, _a, _1, _2)
		];
		action_forward.name("action_forward_declaration");
		//on_error<rethrow>(action_forward, delete_(_a));

		action_def = (
			(("action" >> r_name >> '(') [
				_a = new_<Scope>(_r1)
			])
			>> -(abstract_var(*_a) % ',') >> ')'
			>> ( '{'
				> -( "precondition:" > formula_parser(*_a) )
				> -( "effect:" > +(effect(*_a) > ';') )
				//> "signal:" //*/
			> '}' )
		) [
			phoenix::bind(
				&Scope::define_global<
					Action,
					boost::optional<BooleanExpression *>,
					boost::optional<vector<AbstractEffectAxiom *>>
				>,
				_r1,
				_a, _1, _2, at_c<0>(_3), at_c<1>(_3)
			)
		];
		action_def.name("action_definition");
		//on_error<rethrow>(action_def, delete_(_a));

		effect = boolean_effect(_r1) | numeric_effect(_r1);

		BOOST_SPIRIT_DEBUG_NODES((action)(action_forward)(action_def));
	}

	BooleanExpressionParser formula_parser;
	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<BooleanExpression> boolean_effect;
	EffectParser<NumericExpression> numeric_effect;
	rule<Action *(Scope &)> action;
	rule<Action *(Scope &), locals<Scope *>> action_forward;
	rule<Action *(Scope &), locals<Scope *>> action_def;
};



/******************
* Fluents
******************/

template<class ExprT>
struct FluentParser : grammar<Fluent<ExprT> *(Scope &), locals<Scope *>> {
	FluentParser()
	: FluentParser::base_type(fluent)
	{
		fluent = fluent_forward(_r1) | fluent_def(_r1);
		fluent.name(type_descr<ExprT>() + "_fluent_declaration");

		fluent_forward = (
			(((type_mark<ExprT>() >> "fluent") >> r_name >> '(') [
				_a = new_<Scope>(_r1)
			] )
			>> (abstract_var(*_a) % ',') >> l(')') >> ';'
		) [ // forward declaration
			_val = phoenix::bind(
				&Scope::declare_global<Fluent<ExprT>>,
				_r1,
				_a, _1, _2
			)
		];
		fluent_forward.name(type_descr<ExprT>() + "fluent_forward_declaration");
		on_error<fail>(fluent_forward, delete_(_a));

		fluent_def = (
			(((type_mark<ExprT>() >> "fluent") >> r_name >> '(') [
				_a = new_<Scope>(_r1)
			] )
			>> (abstract_var(*_a) % ',') >> ')'
			>> ( l('{') // definition
				> "initially:"
				> +initially
			> '}' )
		) [
			_val = phoenix::bind(
				&Scope::define_global<
					Fluent<ExprT>,
					const vector<InitialValue<ExprT> *> &
				>,
				_r1,
				_a, _1, _2, _3
			)
		];
		fluent_def.name(type_descr<ExprT>() + "fluent_definition");
		on_error<fail>(fluent_def, delete_(_a));

		initially = (l('(') > -(abstract_constant % ',') > ')' > '=' > r_constant > ';') [
			_val = new_<InitialValue<ExprT>>(_1, _2)
		];
		initially.name("initial_value_mapping");

		r_constant = constant<ExprT>();

		BOOST_SPIRIT_DEBUG_NODES((fluent)(fluent_forward)(fluent_def)(initially));
	}

	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent;
	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent_forward;
	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent_def;
	rule<InitialValue<ExprT> *> initially;
	rule<Constant<ExprT> *> r_constant;
};



/******************
* Complete program
******************/

struct ProgramParser : grammar<Statement *(Scope &)> {
	ProgramParser()
	: ProgramParser::base_type(program)
	{
		program = *( omit[ // Discard attributes, they just register themselves as Globals
			num_fluent(_r1)
			| bool_fluent(_r1)
			| action(_r1)
			| function(_r1)
		] ) >> statement(_r1);

		on_error<rethrow>(program,
			phoenix::bind(&handle_error, _1, _3, _2, _4)
		);

		BOOST_SPIRIT_DEBUG_NODE(program);
	}

	rule<Statement *(Scope &)> program;
	FluentParser<NumericExpression> num_fluent;
	FluentParser<BooleanExpression> bool_fluent;
	ActionParser action;
	AbstractFunctionParser function;
	StatementParser statement;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_GRAMMAR_H_
