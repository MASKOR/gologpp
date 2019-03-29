#include "field_access.h"
#include "statements.h"
#include "types.h"
#include "formula.h"
#include "symbolic_expression.h"
#include "string_expression.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_attr.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/object/construct.hpp>

#include <model/action.h>


namespace gologpp {
namespace parser {


StatementParser &statement_()
{
	static StatementParser rv;
	return rv;
}

rule<Statement *(Scope &)> statement = statement_()(_r1);


template<class ExprT>
PickParser<ExprT>::PickParser()
: PickParser<ExprT>::base_type(pick, type_descr<ExprT>()() + "_pick")
{
	pick = {
		((lit("pick") >> '(') [
			_a = new_<Scope>(_r1)
		] >> var<ExprT, VarDefinitionMode::FORCE>()(*_a)
		> -(lit("in") > '{' > constant % ',' > '}')
		> ')' > statement(*_a)) [
			_val = new_<Pick<ExprT>>(_a, _1, _2, _3)
		],
		type_descr<ExprT>()() + "_pick"
	};
}



StatementParser::StatementParser()
: StatementParser::base_type(statement, "statement")
{
	statement = compound_statement(_r1) | simple_statement(_r1);
	statement.name("statement");

	simple_statement = (test(_r1)
		| boolean_return(_r1) | numeric_return(_r1) | symbolic_return(_r1) | string_return(_r1)
		| compound_return(_r1)
		| numeric_field_assignment(_r1) | boolean_field_assignment(_r1)
		| symbolic_field_assignment(_r1) | string_field_assignment(_r1)
		| compound_field_assignment(_r1)
		| durative_call(_r1)
		| numeric_fluent_assignment(_r1) | boolean_fluent_assignment(_r1)
		| symbolic_fluent_assignment(_r1) | string_fluent_assignment(_r1)
		| compound_fluent_assignment(_r1)
		| action_call(_r1) | procedure_call(_r1)) > ';';
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
		> statement(_r1)
		> (
			("else" > statement(_r1))
			| empty_statement(_r1)
		)
	) [
		_val = new_<Conditional>(_1, _2, _3)
	];
	conditional.name("conditional");

	empty_statement = qi::attr(new_<Block>(new_<Scope>(_r1), construct<vector<Statement *>>()));
	empty_statement.name("empty_statement");

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
	symbolic_return.name("symbolic_return");

	string_return = (lit("return") >> string_expression(_r1)) [
		_val = new_<Return<StringExpression>>(_1)
	];
	string_return.name("string_return");


	durative_call = (
		( (
			lit("start") [ _a = DurativeCall::Hook::START ]
			| lit("stop") [ _a = DurativeCall::Hook::STOP ]
			| lit("finish") [ _a = DurativeCall::Hook::FINISH ]
			| lit("fail") [ _a = DurativeCall::Hook::FAIL ]
		) >> "(" )
		> action_call(_r1) > ")"
	) [
		_val = new_<DurativeCall>(_a, _1)
	];
	durative_call.name("durative_action_call");


	GOLOGPP_DEBUG_NODES((statement)(simple_statement)(compound_statement)
		(block)(choose)(conditional)(search)(solve)(test)(r_while)
		(boolean_return)(numeric_return));
}


} // namespace parser
} // namespace gologpp
