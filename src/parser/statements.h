#ifndef GOLOGPP_PARSER_STATEMENTS_H_
#define GOLOGPP_PARSER_STATEMENTS_H_

#include "utilities.h"
#include "reference.h"
#include "assignment.h"
#include "atoms.h"
#include "formula.h"
#include "arithmetic.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"


namespace gologpp {
namespace parser {


template<class ExprT>
struct PickParser : grammar<Pick<ExprT> *(Scope &), locals<Scope *>> {
	PickParser();

	rule<Pick<ExprT> *(Scope &), locals<Scope *>> pick;
	ConstantParser<ExprT> constant;
};


struct StatementParser : grammar<VoidExpression *(Scope &)> {
	StatementParser();

	rule<VoidExpression *(Scope &)> statement;
	rule<VoidExpression *(Scope &)> simple_statement;
	rule<VoidExpression *(Scope &)> compound_statement;
	rule<Block *(Scope &), locals<Scope *>> block;
	rule<Choose *(Scope &), locals<Scope *>> choose;
	rule<Conditional *(Scope &)> conditional;

	PickParser<BooleanExpression> boolean_pick;
	PickParser<NumericExpression> numeric_pick;
	PickParser<SymbolicExpression> symbolic_pick;
	PickParser<StringExpression> string_pick;

	rule<Search *(Scope &)> search;
	rule<Solve *(Scope &)> solve;
	ReferenceParser<NumericFunction> reward_fn;
	rule<Test *(Scope &)> test;
	rule<While *(Scope &)> r_while;
	rule<Concurrent *(Scope &), locals<Scope *>> concurrent;

	rule<Statement *(Scope &)> empty_statement;

	rule<Return<BooleanExpression> *(Scope &)> boolean_return;
	rule<Return<NumericExpression> *(Scope &)> numeric_return;
	rule<Return<SymbolicExpression> *(Scope &)> symbolic_return;
	rule<Return<StringExpression> *(Scope &)> string_return;
	rule<Return<CompoundExpression> *(Scope &)> compound_return;

	ReferenceParser<Procedure> procedure_call;
	ReferenceParser<Action> action_call;
	rule<DurativeCall *(Scope &), locals<DurativeCall::Hook>> durative_call;

	AssignmentParser<Reference<BooleanFluent>> boolean_fluent_assignment;
	AssignmentParser<Reference<NumericFluent>> numeric_fluent_assignment;
	AssignmentParser<Reference<SymbolicFluent>> symbolic_fluent_assignment;
	AssignmentParser<Reference<StringFluent>> string_fluent_assignment;
	AssignmentParser<Reference<CompoundFluent>> compound_fluent_assignment;

	AssignmentParser<FieldAccess<BooleanExpression>> boolean_field_assignment;
	AssignmentParser<FieldAccess<NumericExpression>> numeric_field_assignment;
	AssignmentParser<FieldAccess<SymbolicExpression>> symbolic_field_assignment;
	AssignmentParser<FieldAccess<StringExpression>> string_field_assignment;
	AssignmentParser<FieldAccess<CompoundExpression>> compound_field_assignment;
};


extern rule<Statement *(Scope &)> statement;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_STATEMENTS_H_

