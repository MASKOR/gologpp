#ifndef GOLOGPP_GRAMMAR_H_
#define GOLOGPP_GRAMMAR_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

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

using iterator = boost::spirit::line_pos_iterator<string::const_iterator>;

template<typename ResultT>
using rule = boost::spirit::qi::rule<iterator, ResultT, ascii::space_type>;

template<typename ResultT>
using grammar = boost::spirit::qi::grammar<iterator, ResultT, ascii::space_type>;



/******************
* Little helpers
******************/

template<class T>
static inline auto l(T x)
{ return qi::lit(x); }

template<class T>
static inline auto _r(T &&x)
{ return boost::phoenix::ref(std::forward<T>(x)); }


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
	VariableParser() : VariableParser::base_type(variable, type_descr<ExpressionT>() + " variable") {
		variable = type_mark<ExpressionT>() >> r_name [
			_val = bind(&Scope::variable<ExpressionT>, _r1, _1)
		];
	}

	rule<shared_ptr<Variable<ExpressionT>>(Scope &)> variable;
};

static VariableParser<BooleanExpression> bool_var;
static VariableParser<NumericExpression> num_var;



static rule<BooleanConstant *()> bool_constant( (
	qi::string("true") | qi::string("false")
) [
	_val = new_<BooleanConstant>(_1)
], "boolean constant");


static rule<NumericConstant *()> num_constant(float_ [
	_val = new_<NumericConstant>(_1)
], "numeric constant");


static rule<shared_ptr<AbstractVariable> (Scope &)> abstract_var(
	bool_var(_r1) [ _val = phoenix::bind(
		&std::dynamic_pointer_cast<AbstractVariable, BooleanVariable>,
		_1
	) ]
	| num_var(_r1) [ _val = phoenix::bind(
		&std::dynamic_pointer_cast<AbstractVariable, NumericVariable>,
		_1
	) ]
, "any variable");



/******************
* Constants
******************/

template<class ExprT>
static rule<Constant<ExprT> *()> &constant();

template<>
rule<Constant<NumericExpression> *()> &constant<NumericExpression>()
{ return num_constant; }

template<>
rule<Constant<BooleanExpression> *()> &constant<BooleanExpression>()
{ return bool_constant; }


static rule<AbstractConstant *()> abstract_constant(bool_constant | num_constant, "any constant");



/******************
* General atoms
******************/

static rule<Expression *(Scope &)> atom(
	bool_var(_r1) [ _val = new_<Reference<BooleanVariable>>(_1, _r1) ]
	| num_var(_r1) [ _val = new_<Reference<NumericVariable>>(_1, _r1) ]
	| bool_constant [ _val = _1 ]
	| num_constant [ _val = _1 ]
, "any atom");



/******************
* References
******************/

template<class ExpressionT>
struct ReferenceParser : grammar<Reference<ExpressionT> *(Scope &)> {
	ReferenceParser() : ReferenceParser::base_type(pred_ref, string("reference to ") + typeid(ExpressionT).name())
	{
		pred_ref = (r_name > "(" > -( (
			attr_cast<Expression *>(pred_ref(_r1)) | atom(_r1)
		) %  ",") > ")"
		) [
			_val = new_<Reference<ExpressionT>>(_1, _r1, _2),
			if_(!phoenix::bind(&Reference<ExpressionT>::bound, _val)) [
				// Target doesn't have the given ExpressionT, so fail this parser
				_pass = false
			]
		];

		on_error<fail>(pred_ref, delete_(_val));
	}

	rule<Reference<ExpressionT> *(Scope &)> pred_ref;
};



/*********************
* Numeric expressions
*********************/

struct NumericExpressionParser : grammar<NumericExpression *(Scope &)> {
	NumericExpressionParser() : NumericExpressionParser::base_type(expression, "numeric expression")
	{
		expression = binary_expr(_r1) | unary_expr(_r1);
		expression.name("numeric expression");

		unary_expr = brace(_r1) | num_constant | num_var_ref(_r1) | num_reference(_r1);
		unary_expr.name("unary numeric expression");

		binary_expr = (
			(unary_expr(_r1) >> arith_operator) > expression(_r1)
		) [
			_val = new_<ArithmeticOperation>(at_c<0>(_1), at_c<1>(_1), _2)
		];
		binary_expr.name("binary numeric expression");

		brace = '(' >> expression(_r1) >> ')';
		brace.name("braced numeric expression");

		arith_operator =
			qi::string("+") [ _val = val(ArithmeticOperation::ADDITION) ]
			| qi::string("-") [ _val = val(ArithmeticOperation::SUBTRACTION) ]
			| qi::string("/") [ _val = val(ArithmeticOperation::DIVISION) ]
			| qi::string("*") [ _val = val(ArithmeticOperation::MULTIPLICATION) ]
			| qi::string("**") [ _val = val(ArithmeticOperation::POWER) ]
			| qi::string("%") [ _val = val(ArithmeticOperation::MODULO) ]
		;
		arith_operator.name("arithmetic operator");

		num_var_ref = num_var(_r1) [ _val = new_<Reference<NumericVariable>>(_1, _r1) ];
		num_var_ref.name("reference to numeric variable");
	}

	rule<NumericExpression *(Scope &)> expression;
	rule<NumericExpression *(Scope &)> binary_expr;
	rule<NumericExpression *(Scope &)> operation;
	rule<NumericExpression *(Scope &)> brace;
	rule<NumericExpression *(Scope &)> num_var_ref;
	rule<NumericExpression *(Scope &)> unary_expr;
	rule<ArithmeticOperation::Operator()> arith_operator;
	ReferenceParser<NumericExpression> num_reference;
};



/*********************
* Boolean expressions
*********************/

struct BooleanExpressionParser : grammar<BooleanExpression *(Scope &)> {
	BooleanExpressionParser()
	: BooleanExpressionParser::base_type(expression, "boolean expression")
	, scope(nullptr)
	{
		expression = binary_expr(_r1) | unary_expr(_r1);
		expression.name("boolean expression");

		unary_expr = quantification(_r1) | negation(_r1) | bool_constant
			| bool_var_ref(_r1) | brace(_r1) | num_comparison(_r1) | bool_reference(_r1);
		unary_expr.name("unary boolean expression");

		binary_expr = (
			(unary_expr(_r1) >> bool_op) > expression(_r1)
		) [
			_val = new_<BooleanOperation>(at_c<0>(_1), at_c<1>(_1), _2, _r1)
		];
		binary_expr.name("binary boolean expression");

		num_comparison = (
			(num_expression(_r1) >> num_cmp_op) > num_expression(_r1)
		) [
			_val = new_<Comparison>(at_c<0>(_1), at_c<1>(_1), _2, _r1)
		];
		num_comparison.name("numeric comparison");

		quantification = ( (quantification_op > '(') [
			_r(scope) = new_<Scope>(nullptr, _r1)
		] > abstract_var(*_r(scope)) > ')' > expression(_r1)) [
			_val = new_<Quantification>(_r(scope), _1, _2, _3)
		];
		quantification.name("quantification");
		on_error<fail>(quantification, delete_(_r(scope)));

		quantification_op = qi::string("exists") [ _val = val(QuantificationOperator::EXISTS) ]
			| qi::string("forall") [ _val = val(QuantificationOperator::FORALL) ];
		quantification_op.name("quantification operator");

		bool_op =
			qi::string("==") [ _val = val(BooleanOperator::IFF) ]
			| qi::string("!=") [ _val = val(BooleanOperator::XOR) ]
			| qi::string("&") [ _val = val(BooleanOperator::AND) ]
			| qi::string("|") [ _val = val(BooleanOperator::OR) ]
			| qi::string("->") [ _val = val(BooleanOperator::IMPLIES) ]
		;
		bool_op.name("boolean operator");

		num_cmp_op =
			qi::string(">") [ _val = val(ComparisonOperator::GT) ]
			| qi::string(">=") [ _val = val(ComparisonOperator::GE) ]
			| qi::string("<=") [ _val = val(ComparisonOperator::LE) ]
			| qi::string("<") [ _val = val(ComparisonOperator::LT) ]
			| qi::string("==") [ _val = val(ComparisonOperator::EQ) ]
			| qi::string("!=") [ _val = val(ComparisonOperator::NEQ) ]
		;
		num_cmp_op.name("numeric comparison operator");

		negation = '!' > unary_expr(_r1) [
			_val = new_<Negation>(construct<unique_ptr<BooleanExpression>>(_1), _r1)
		];
		negation.name("negation");

		brace = '(' >> expression(_r1) >> ')';
		brace.name("braced boolean expression");

		bool_var_ref = bool_var(_r1) [ _val = new_<Reference<BooleanVariable>>(_1, _r1) ];
		bool_var_ref.name("reference to boolean variable");
	}

	Scope *scope;
	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> unary_expr;
	rule<BooleanExpression *(Scope &)> binary_expr;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
	rule<BooleanExpression *(Scope &)> bool_var_ref;
	rule<BooleanExpression *(Scope &)> quantification;
	rule<QuantificationOperator()> quantification_op;
	ReferenceParser<BooleanExpression> bool_reference;
	rule<BooleanOperator()> bool_op;
	rule<BooleanExpression *(Scope &)> num_comparison;
	NumericExpressionParser num_expression;
	rule<ComparisonOperator()> num_cmp_op;
};



/*********************
* General expressions
*********************/

template<class ExpressionT>
struct ExpressionParser;

template<>
struct ExpressionParser<BooleanExpression> : BooleanExpressionParser {
	using BooleanExpressionParser::BooleanExpressionParser;
};

template<>
struct ExpressionParser<NumericExpression> : NumericExpressionParser {
	using NumericExpressionParser::NumericExpressionParser;
};



/******************
* Assignments
******************/

template<class LhsT>
struct AssignmentParser;

template<class ExpressionT>
struct AssignmentParser<Fluent<ExpressionT>> : grammar<Assignment<Fluent<ExpressionT>> *(Scope &)> {
	AssignmentParser()
	: AssignmentParser<Fluent<ExpressionT>>::base_type(assignment, string("assignment to ") + type_descr<ExpressionT>() + " fluent")
	{
		assignment = (fluent_ref(_r1) >> "=" > expression(_r1)) [
			_val = new_<Assignment<Fluent<ExpressionT>>>(_1, _2, _r1)
		];
	}

	rule<Assignment<Fluent<ExpressionT>> *(Scope &)> assignment;
	ReferenceParser<Fluent<ExpressionT>> fluent_ref;
	ExpressionParser<ExpressionT> expression;
};

template<class ExpressionT>
struct AssignmentParser<Variable<ExpressionT>> : grammar<Assignment<Variable<ExpressionT>> *(Scope &)> {
	AssignmentParser()
	: AssignmentParser<Variable<ExpressionT>>::base_type(assignment, string("assignment to ") + type_descr<ExpressionT>() + " variable")
	{
		assignment = (var_ref(_r1) >> "=" > expression(_r1)) [
			_val = new_<Assignment<Variable<ExpressionT>>>(_1, _2, _r1)
		];

		var_ref = var(_r1) [ _val = new_<Reference<Variable<ExpressionT>>>(_1, _r1) ];
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
	StatementParser() : StatementParser::base_type(statement, "statement")
	{
		statement = compound_statement(_r1) | (simple_statement(_r1) > ';');
		statement.name("statement");

		simple_statement = test(_r1) | boolean_return(_r1) | numeric_return(_r1)
			| numeric_var_assignment(_r1) | bool_var_assignment(_r1)
			| numeric_fluent_assignment(_r1) | bool_fluent_assignment(_r1)
			| action_call(_r1) | procedure_call(_r1);
		simple_statement.name("simple statement");

		compound_statement = block(_r1) | choose(_r1) | conditional(_r1)
			| pick(_r1) | search(_r1) | r_while(_r1);
		compound_statement.name("compound statement");


		block = (l('{') [
			_r(scope) = new_<Scope>(nullptr, _r1)
		] > +statement(*_r(scope)) > '}') [
			_val = new_<Block>(_r(scope), _1)
		];
		block.name("block");
		on_error<fail>(block, delete_(_r(scope)));


		choose = ((l("choose") > '{') [
			_r(scope) = new_<Scope>(nullptr, _r1)
		] > +statement(*_r(scope)) > '}') [
			_val = new_<Choose>(_r(scope), _1)
		];
		choose.name("choose");
		on_error<fail>(choose, delete_(_r(scope)));


		conditional = (l("if") > '(' > boolean_expression(_r1) > ')'
			> statement(_r1) > -("else" > statement(_r1))
		) [
			_val = new_<Conditional>(_1, _2, _3, _r1)
		];
		conditional.name("conditional");


		pick = ((l("pick") > '(') [
			_r(scope) = new_<Scope>(nullptr, _r1)
		] > abstract_var(*_r(scope)) > ')' > statement(*_r(scope))) [
			_val = new_<Pick>(_r(scope), _1, _2)
		];
		pick.name("pick");
		on_error<fail>(pick, delete_(_r(scope)));


		search = (l("search") > statement(_r1)) [
			_val = new_<Search>(_1, _r1)
		];
		search.name("search");

		test = (l("test") > '(' > boolean_expression(_r1) > ')') [
			_val = new_<Test>(_1, _r1)
		];
		test.name("test");

		r_while = (l("while") > '(' > boolean_expression(_r1) > ')' > statement(_r1)) [
			_val = new_<While>(_1, _2, _r1)
		];
		r_while.name("while");

		boolean_return = (l("return") > boolean_expression(_r1)) [
			_val = new_<Return<BooleanExpression>>(_1, _r1)
		];
		boolean_return.name("boolean return");

		numeric_return = (l("return") > numeric_expression(_r1)) [
			_val = new_<Return<NumericExpression>>(_1, _r1)
		];
		numeric_return.name("numeric return");
	}

	Scope *scope;

	rule<Statement *(Scope &)> statement;
	rule<Statement *(Scope &)> simple_statement;
	rule<Statement *(Scope &)> compound_statement;
	rule<Block *(Scope &)> block;
	rule<Choose *(Scope &)> choose;
	rule<Conditional *(Scope &)> conditional;
	AssignmentParser<BooleanFluent> bool_fluent_assignment;
	AssignmentParser<NumericFluent> numeric_fluent_assignment;
	AssignmentParser<BooleanVariable> bool_var_assignment;
	AssignmentParser<NumericVariable> numeric_var_assignment;
	rule<Pick *(Scope &)> pick;
	rule<Search *(Scope &)> search;
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
struct FunctionParser : grammar<Function<ExpressionT> *(Scope &)> {
	FunctionParser()
	: FunctionParser::base_type(function, type_descr<ExpressionT>() + " function definition")
	, scope(nullptr)
	{
		function = (
			((type_mark<ExpressionT>() >> "function") > r_name > '(') [
				// TODO: cleanup on fail!
				_r(scope) = new_<Scope>(nullptr)
			]
			> *abstract_var(*_r(scope)) > ')' > -(statement(*_r(scope)))
		) [
			_val = phoenix::bind(
				&Scope::define_global<Function<ExpressionT>, boost::optional<Statement *>>,
				_r1,
				_r(scope),
				_1, _2, _3
			)
		];
	}

	rule<Function<ExpressionT> *(Scope &)> function;
	Scope *scope;
	StatementParser statement;
};



struct AbstractFunctionParser : grammar<AbstractFunction *(Scope &)> {
	AbstractFunctionParser()
	: AbstractFunctionParser::base_type(function, "any function definition")
	{
		function = bool_func(_r1) | num_func(_r1);
	}

	rule<AbstractFunction *(Scope &)> function;
	FunctionParser<NumericExpression> num_func;
	FunctionParser<BooleanExpression> bool_func;
};



/******************
* Effect axioms
******************/

struct EffectParser : grammar<AbstractEffectAxiom *(AbstractAction *, Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect, "effect axiom")
	{
		effect = boolean_effect(_r1, _r2) | numeric_effect(_r1, _r2);

		//FIXME: disambiguate cases like fluent(a1) = func(a1)!
		//Requires forward declarations!

		boolean_effect = (
			-(boolean_expression(_r2) >> "->")
			>> bool_fluent_ref(_r2)
			>> '=' >> boolean_expression(_r2)
		) [
			_val = new_<EffectAxiom<BooleanExpression>>(_r1, _1, _2, _3)
		];

		numeric_effect = (
			-(boolean_expression(_r2) >> "->")
			>> num_fluent_ref(_r2)
			>> '=' >> numeric_expression(_r2)
		) [
			_val = new_<EffectAxiom<NumericExpression>>(_r1, _1, _2, _3)
		];
	}

	rule<AbstractEffectAxiom *(AbstractAction *, Scope &)> effect;
	BooleanExpressionParser boolean_expression;
	NumericExpressionParser numeric_expression;
	ReferenceParser<BooleanFluent> bool_fluent_ref;
	ReferenceParser<NumericFluent> num_fluent_ref;
	rule<EffectAxiom<BooleanExpression> *(AbstractAction *, Scope &)> boolean_effect;
	rule<EffectAxiom<NumericExpression> *(AbstractAction *, Scope &)> numeric_effect;
};



/******************
* Actions
******************/

struct ActionParser : grammar<Action *(Scope &)> {
	ActionParser()
	: ActionParser::base_type(action, "action definition")
	, scope(nullptr)
	{
		action = (
			(("action" > r_name > '(') [ delete_(_r(scope)), _r(scope) = new_<Scope>(nullptr) ])
			// CRUCIAL detail: use lazy dereference, i.e. *_r(scope), not _r(*scope).
			// Otherwise, we create a reference to the contents of an uninitialized pointer.
			> abstract_var(*_r(scope)) % ',' > ')'
		) [
			_val = phoenix::bind(
				&Scope::declare_global<Action>,
				_r1, _r(scope), _1, _2
			)
		]
		> -( '{'
			> -( "precondition:" > formula_parser(*_r(scope)) [
				phoenix::bind(
					&Action::set_precondition,
					*_val,
					_1
				)
			] )
			> -( "effect:" > (effect_parser(_val, *_r(scope)) [
				phoenix::bind(
					&Action::add_effect,
					*_val,
					_1
				)
			] % ';') )
		//>> "signal:" //*/
		> '}' )
		;
	}

	BooleanExpressionParser formula_parser;
	EffectParser effect_parser;
	Scope *scope;
	rule<Action *(Scope &)> action;
};



/******************
* Fluents
******************/

template<class ExprT>
struct FluentParser : grammar<AbstractFluent *(Scope &)> {
	FluentParser()
	: FluentParser::base_type(fluent, "fluent definition")
	, scope(nullptr)
	{
		fluent = (
			(((type_mark<ExprT>() >> "fluent") > r_name > '(') [ delete_(_r(scope)), _r(scope) = new_<Scope>(nullptr) ])
			> (abstract_var(*_r(scope)) % ',') > ')' > -('=' > constant<ExprT>()) > ';'
		) [
			_val = phoenix::bind(
				&Scope::define_global<
					Fluent<ExprT>,
					boost::optional<Constant<ExprT> *>
				>,
				_r1,
				_r(scope),
				_1, _2, _3
			)
		];
	}

	rule<AbstractFluent *(Scope &)> fluent;
	Scope *scope;
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
		] ) > statement(_r1);

		on_error<fail>(program,
			phoenix::bind(&handle_error, _1, _3, _2, _4)
		);
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
