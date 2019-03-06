#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "expressions.h"
#include "atoms.h"


namespace gologpp {
namespace parser {


extern ReferenceParser<Fluent<CompoundExpression>> compound_fluent_ref;
extern ReferenceParser<Function<CompoundExpression>> compound_function_ref;
extern FieldAccessParser<CompoundExpression> compound_field_access;


template<>
struct ExpressionParser<CompoundExpression> : public grammar<CompoundExpression *(Scope &)> {
	ExpressionParser();

	rule<CompoundExpression *(Scope &)> expression;
	rule<Reference<Variable<CompoundExpression>> *(Scope &)> var_ref;
};

extern ExpressionParser<CompoundExpression> compound_expression;


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
