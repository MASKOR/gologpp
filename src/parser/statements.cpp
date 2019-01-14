#include "statements.h"
#include "types.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <model/action.h>

namespace gologpp {
namespace parser {



template<class ExprT>
inline rule<Pick<ExprT> *(Scope &), locals<Scope *>> StatementParser::pick_()
{
	return {
		((lit("pick") >> '(') [
			_a = new_<Scope>(_r1)
		] >> var<ExprT, true>()(*_a)
		> -(lit("in") > '{' > constant<ExprT>() % ',' > '}')
		> ')' > statement(*_a)) [
			_val = new_<Pick<ExprT>>(_a, _1, _2, _3)
		],
		type_descr<ExprT>() + "_pick"
	};
}



StatementParser::StatementParser()
: StatementParser::base_type(statement, "statement")
{
	statement = compound_statement(_r1) | simple_statement(_r1);
	statement.name("statement");

	simple_statement = (test(_r1)
		| boolean_return(_r1) | numeric_return(_r1) | symbolic_return(_r1) | string_return(_r1)
		| numeric_var_assignment(_r1) | boolean_var_assignment(_r1)
		| symbolic_var_assignment(_r1) | string_var_assignment(_r1)
		| numeric_fluent_assignment(_r1) | boolean_fluent_assignment(_r1)
		| symbolic_fluent_assignment(_r1) | string_fluent_assignment(_r1)
		| action_call(_r1) | procedure_call(_r1) | durative_call(_r1)) > ';';
	simple_statement.name("simple_statement");

	compound_statement = block(_r1) | choose(_r1) | conditional(_r1)
		| boolean_pick(_r1)
		| numeric_pick(_r1)
		| symbolic_pick(_r1)
		| string_pick(_r1)
		| solve(_r1) | search(_r1) | r_while(_r1)
		| concurrent(_r1);
	compound_statement.name("compound_statement");


	block = (lit('{') [
		_a = new_<Scope>(_r1)
	] > +statement(*_a) > '}') [
		_val = new_<Block>(_a, _1)
	];
	block.name("block");
	on_error<rethrow>(block, delete_(_a));


	choose = ((lit("choose") > '{') [
		_a = new_<Scope>(_r1)
	] > +statement(*_a) > '}') [
		_val = new_<Choose>(_a, _1)
	];
	choose.name("choose");
	on_error<rethrow>(choose, delete_(_a));


	conditional = (lit("if") > '(' > boolean_expression(_r1) > ')'
		> statement(_r1) > -("else" > statement(_r1))
	) [
		_val = new_<Conditional>(_1, _2, _3)
	];
	conditional.name("conditional");

	boolean_pick = pick_<BooleanExpression>();
	numeric_pick = pick_<NumericExpression>();
	symbolic_pick = pick_<SymbolicExpression>();

	search = (lit("search") > statement(_r1)) [
		_val = new_<Search>(_1)
	];
	search.name("search");

	solve = (lit("solve") > '('
		> numeric_expression(_r1) > ',' > reward_fn(_r1)
	> ')' > statement(_r1)) [
		_val = new_<Solve>(_1, _2, _3)
	];

	test = (lit("test") > '(' > boolean_expression(_r1) > ')') [
		_val = new_<Test>(_1)
	];
	test.name("test");

	r_while = (lit("while") > '(' > boolean_expression(_r1) > ')' > statement(_r1)) [
		_val = new_<While>(_1, _2)
	];
	r_while.name("while");

	concurrent = ((lit("concurrent") > '{') [
		_a = new_<Scope>(_r1)
	] > +statement(*_a) > '}') [
		_val = new_<Concurrent>(_a, _1)
	];
	choose.name("concurrent");
	on_error<rethrow>(concurrent, delete_(_a));

	boolean_return = (lit("return") >> boolean_expression(_r1)) [
		_val = new_<Return<BooleanExpression>>(_1)
	];
	boolean_return.name("boolean_return");

	numeric_return = (lit("return") >> numeric_expression(_r1)) [
		_val = new_<Return<NumericExpression>>(_1)
	];
	numeric_return.name("numeric_return");

	symbolic_return = (lit("return") >> symbolic_expression(_r1)) [
		_val = new_<Return<SymbolicExpression>>(_1)
	];
	numeric_return.name("symbolic_return");

	durative_call = (
		( (
			lit("start") [ _a = DurativeCall::Type::START ]
			| lit("stop") [ _a = DurativeCall::Type::STOP ]
			| lit("finish") [ _a = DurativeCall::Type::FINISH ]
			| lit("fail") [ _a = DurativeCall::Type::FAIL ]
		) >> "(" )
		> action_call(_r1) > ")"
	) [
		_val = new_<DurativeCall>(_a, _1)
	];
	durative_call.name("durative_action_call");


	BOOST_SPIRIT_DEBUG_NODES((statement)(simple_statement)(compound_statement)
		(block)(choose)(conditional)(search)(solve)(test)(r_while)
		(boolean_return)(numeric_return));
}


} // namespace parser
} // namespace gologpp
