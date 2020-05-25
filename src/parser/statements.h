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

#ifndef GOLOGPP_PARSER_STATEMENTS_H_
#define GOLOGPP_PARSER_STATEMENTS_H_

#include "utilities.h"
#include "reference.h"
#include "assignment.h"
#include "formula.h"
#include "arithmetic.h"


namespace gologpp {
namespace parser {



struct StatementParser : grammar<Instruction *(Scope &)> {
	StatementParser();

	rule<Instruction *(Scope &)> statement;
	rule<Instruction *(Scope &)> simple_statement;
	rule<Instruction *(Scope &)> compound_statement;
	rule<Block *(Scope &), locals<Scope *>> block;
	rule<Choose *(Scope &), locals<Scope *>> choose;
	rule<Conditional<Instruction> *(Scope &)> conditional;
	rule<Pick *(Scope &), locals<Scope *, shared_ptr<Variable>>> pick;

	rule<Search *(Scope &)> search;
	rule<Solve *(Scope &)> solve;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Concurrent *(Scope &), locals<Scope *>> concurrent;

	rule<ListPop *(Scope &), locals<ListOpEnd>> list_pop;
	rule<ListPush *(Scope &), locals<ListOpEnd, shared_ptr<const Type>>> list_push;

	rule<During *(Scope &)> during;

	rule<Instruction *(Scope &)> empty_statement;

	ReferenceParser<Action> action_call;
	ReferenceParser<Procedure> procedure_call;
	rule<DurativeCall *(Scope &), locals<DurativeCall::Hook>> durative_call;

	AssignmentParser<Reference<Fluent>> fluent_assignment;
	AssignmentParser<FieldAccess> field_assignment;
	AssignmentParser<ListAccess> list_element_assignment;

	BooleanExpressionParser boolean_expression;
	NumericExpressionParser numeric_expression;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_STATEMENTS_H_

