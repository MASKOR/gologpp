#ifndef GOLOGPP_PARSER_STATEMENTS_H_
#define GOLOGPP_PARSER_STATEMENTS_H_

#include "utilities.h"
#include "reference.h"
#include "formula.h"
#include "arithmetic.h"
#include "assignment.h"
#include "symbolic_expression.h"


namespace gologpp {
namespace parser {



struct StatementParser : grammar<Statement *(Scope &)> {
	StatementParser();

	rule<Statement *(Scope &)> statement;
	rule<Statement *(Scope &)> simple_statement;
	rule<Statement *(Scope &)> compound_statement;
	rule<Block *(Scope &), locals<Scope *>> block;
	rule<Choose *(Scope &), locals<Scope *>> choose;
	rule<Conditional *(Scope &)> conditional;
	rule<Pick<BooleanExpression> *(Scope &), locals<Scope *>> boolean_pick;
	rule<Pick<NumericExpression> *(Scope &), locals<Scope *>> numeric_pick;
	rule<Pick<SymbolicExpression> *(Scope &), locals<Scope *>> symbolic_pick;
	rule<Search *(Scope &)> search;
	rule<Solve *(Scope &)> solve;
	ReferenceParser<NumericFunction> reward_fn;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Return<BooleanExpression> *(Scope &)> boolean_return;
	rule<Return<NumericExpression> *(Scope &)> numeric_return;
	rule<Return<SymbolicExpression> *(Scope &)> symbolic_return;
	ReferenceParser<Procedure> procedure_call;
	ReferenceParser<Action> action_call;
	BooleanExpressionParser boolean_expression;
	NumericExpressionParser numeric_expression;
	SymbolicExpressionParser symbolic_expression;
	AssignmentParser<Fluent<BooleanExpression>> boolean_fluent_assignment;
	AssignmentParser<Fluent<NumericExpression>> numeric_fluent_assignment;
	AssignmentParser<Fluent<SymbolicExpression>> symbolic_fluent_assignment;
	AssignmentParser<Variable<BooleanExpression>> boolean_var_assignment;
	AssignmentParser<Variable<BooleanExpression>> numeric_var_assignment;
	AssignmentParser<Variable<SymbolicExpression>> symbolic_var_assignment;

	template<class ExprT>
	inline rule<Pick<ExprT> *(Scope &), locals<Scope *>> pick_();
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_STATEMENTS_H_

