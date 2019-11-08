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

#include "statements.h"

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
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include "types.h"
#include "formula.h"
#include "expressions.h"
#include "variable.h"
#include "value.h"

namespace gologpp {
namespace parser {


static string element_type_name(const Expression *list_expr)
{ return dynamic_cast<const ListType &>(list_expr->type()).element_type().name(); }


StatementParser::StatementParser()
: StatementParser::base_type(statement, "statement")
{
	statement = compound_statement(_r1) | simple_statement(_r1);
	statement.name("statement");

	simple_statement = (test(_r1)
		| return_stmt(_r1)
		| durative_call(_r1)
		| field_assignment(_r1)
		| fluent_assignment(_r1)
		| list_element_assignment(_r1)
		| action_call(_r1)
		| list_pop(_r1)
		| list_push(_r1)
		| typed_reference<Function>()(_r1, VoidType::name()))
		> ';';
	simple_statement.name("simple_statement");

	compound_statement = block(_r1) | choose(_r1) | conditional(_r1)
		| pick(_r1)
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

	pick = (
		(lit("pick") > '(') [
			_a = new_<Scope>(_r1)
		] > var_decl()(*_a) [
			_b = _1
		] > -(lit("in") > '{'
		> value()(phoenix::bind(&Expression::type_name, *_b), false) % ','
		> '}')
		> ')' > statement(*_a)
	) [
		_val = new_<Pick>(_a, _1, _2, _3)
	];
	pick.name("pick");

	empty_statement = qi::attr(new_<Block>(new_<Scope>(_r1), construct<vector<Expression *>>()));
	empty_statement.name("empty_statement");

	search = (lit("search") > statement(_r1)) [
		_val = new_<Search>(_1)
	];
	search.name("search");

	solve = (lit("solve") > '('
		> numeric_expression(_r1) > ',' > typed_reference<Function>()(_r1, NumberType::name())
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

	list_pop = (
		(
			lit("pop_front") [ _a = ListOpEnd::FRONT ]
			| lit("pop_back") [ _a = ListOpEnd::BACK ]
		) > '(' > list_expression(_r1) > ')'
	) [
		_val = new_<ListPop>(_1, _a)
	];
	list_pop.name("list_pop");

	list_push = (
		(
			lit("push_front") [ _a = ListOpEnd::FRONT ]
			| lit("push_back") [ _a = ListOpEnd::BACK ]
		) > '('
		> list_expression(_r1) [
			_b = phoenix::bind(&element_type_name, _1)
		]
		> ',' > typed_expression()(_r1, _b)
		> ')'
	) [
		_val = new_<ListPush>(_1, _a, _2)
	];
	list_push.name("list_push");

	return_stmt = (lit("return") >> value_expression()(_r1)) [
		_val = new_<Return>(_1)
	];
	return_stmt.name("return");


	durative_call = (
		( (
			lit("start") [ _a = DurativeCall::Hook::START ]
			| lit("cancel") [ _a = DurativeCall::Hook::CANCEL ]
			| lit("finish") [ _a = DurativeCall::Hook::FINISH ]
			| lit("fail") [ _a = DurativeCall::Hook::FAIL ]
		) >> "(" )
		> action_call(_r1) > ")"
	) [
		_val = new_<DurativeCall>(_a, _1)
	];
	durative_call.name("durative_action_call");


	GOLOGPP_DEBUG_NODES(
		//(statement)(simple_statement)(compound_statement)
		//(block)(choose)(conditional)(search)(solve)(test)(r_while)
		//(return_stmt)(pick)
		//(list_pop)(list_push)
	)
}


} // namespace parser
} // namespace gologpp
