#ifndef GOLOGPP_PARSER_STATEMENTS_H_
#define GOLOGPP_PARSER_STATEMENTS_H_

#include "utilities.h"
#include "reference.h"
#include "formula.h"
#include "arithmetic.h"
#include "assignment.h"
#include "symbolic_expression.h"
#include "string_expression.h"


namespace gologpp {
namespace parser {



struct StatementParser : grammar<VoidExpression *(Scope &)> {
	StatementParser();

	rule<VoidExpression *(Scope &)> statement;
	rule<VoidExpression *(Scope &)> simple_statement;
	rule<VoidExpression *(Scope &)> compound_statement;
	rule<Block *(Scope &), locals<Scope *>> block;
	rule<Choose *(Scope &), locals<Scope *>> choose;
	rule<Conditional *(Scope &)> conditional;

	rule<Pick<BooleanExpression> *(Scope &), locals<Scope *>> boolean_pick;
	rule<Pick<NumericExpression> *(Scope &), locals<Scope *>> numeric_pick;
	rule<Pick<SymbolicExpression> *(Scope &), locals<Scope *>> symbolic_pick;
	rule<Pick<StringExpression> *(Scope &), locals<Scope *>> string_pick;

	rule<Search *(Scope &)> search;
	rule<Solve *(Scope &)> solve;
	ReferenceParser<NumericFunction> reward_fn;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Concurrent *(Scope &), locals<Scope *>> concurrent;

	rule<Return<BooleanExpression> *(Scope &)> boolean_return;
	rule<Return<NumericExpression> *(Scope &)> numeric_return;
	rule<Return<SymbolicExpression> *(Scope &)> symbolic_return;
	rule<Return<StringExpression> *(Scope &)> string_return;

	ReferenceParser<Procedure> procedure_call;
	ReferenceParser<Action> action_call;
	rule<DurativeCall *(Scope &), locals<DurativeCall::Type>> durative_call;

	ExpressionParser<BooleanExpression> boolean_expression;
	ExpressionParser<NumericExpression> numeric_expression;
	ExpressionParser<SymbolicExpression> symbolic_expression;
	ExpressionParser<StringExpression> string_expression;

	AssignmentParser<Fluent<BooleanExpression>> boolean_fluent_assignment;
	AssignmentParser<Fluent<NumericExpression>> numeric_fluent_assignment;
	AssignmentParser<Fluent<SymbolicExpression>> symbolic_fluent_assignment;
	AssignmentParser<Fluent<StringExpression>> string_fluent_assignment;

	AssignmentParser<Variable<BooleanExpression>> boolean_var_assignment;
	AssignmentParser<Variable<BooleanExpression>> numeric_var_assignment;
	AssignmentParser<Variable<SymbolicExpression>> symbolic_var_assignment;
	AssignmentParser<Variable<StringExpression>> string_var_assignment;

	template<class ExprT>
	inline rule<Pick<ExprT> *(Scope &), locals<Scope *>> pick_();
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_STATEMENTS_H_

