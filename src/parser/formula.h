#ifndef GOLOGPP_PARSER_FORMULA_H_
#define GOLOGPP_PARSER_FORMULA_H_

#include <model/formula.h>

#include "utilities.h"
#include "expressions.h"
#include "arithmetic.h"
#include "symbolic_expression.h"
#include "string_expression.h"
#include "atoms.h"


namespace gologpp {
namespace parser {


template<class ExprT>
struct ComparisonParser : grammar<Comparison<ExprT> *(Scope &)> {
	ComparisonParser();

	rule<Comparison<ExprT> *(Scope &)> comparison;
	rule<ComparisonOperator()> cmp_op;
	ExpressionParser<ExprT> expression;
};


extern ReferenceParser<BooleanFluent> boolean_fluent_ref;
extern ReferenceParser<BooleanFunction> boolean_function_ref;
extern FieldAccessParser<BooleanExpression> boolean_field_access;
extern ConstantParser<BooleanExpression> boolean_constant;
extern ComparisonParser<NumericExpression> numeric_comparison;
extern ComparisonParser<SymbolicExpression> symbolic_comparison;
extern ComparisonParser<StringExpression> string_comparison;


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
	rule<BooleanOperator()> bool_op;
};


extern ExpressionParser<BooleanExpression> boolean_expression;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FORMULA_H_

