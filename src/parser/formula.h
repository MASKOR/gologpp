#ifndef GOLOGPP_PARSER_FORMULA_H_
#define GOLOGPP_PARSER_FORMULA_H_

#include <model/formula.h>

#include "utilities.h"
#include "expressions.h"
#include "arithmetic.h"
#include "symbolic_expression.h"
#include "string_expression.h"


namespace gologpp {
namespace parser {


template<class ExprT>
struct ComparisonParser : grammar<Comparison<ExprT> *(Scope &)> {
	ComparisonParser();

	rule<Comparison<ExprT> *(Scope &)> comparison;
	rule<ComparisonOperator()> cmp_op;
	ExpressionParser<ExprT> expression;
};


template<>
struct ExpressionParser<BooleanExpression> : grammar<BooleanExpression *(Scope &)> {
	ExpressionParser();

	rule<BooleanExpression *(Scope &)> expression;
	rule<BooleanExpression *(Scope &)> unary_expr;
	rule<BooleanExpression *(Scope &)> binary_expr;
	rule<BooleanExpression *(Scope &)> negation;
	rule<BooleanExpression *(Scope &)> brace;
	rule<BooleanExpression *(Scope &)> bool_var_ref;
	rule<BooleanExpression *(Scope &), locals<Scope *>> quantification;
	rule<QuantificationOperator()> quantification_op;
	unique_ptr<ReferenceParser<BooleanFluent>> bool_fluent_ref;
	unique_ptr<ReferenceParser<BooleanFunction>> bool_function_ref;
	unique_ptr<FieldAccessParser<BooleanExpression>> field_access;
	rule<BooleanOperator()> bool_op;
	ComparisonParser<NumericExpression> numeric_comparison;
	ComparisonParser<SymbolicExpression> symbolic_comparison;
	ComparisonParser<StringExpression> string_comparison;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FORMULA_H_

