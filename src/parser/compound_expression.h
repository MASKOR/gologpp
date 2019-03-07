#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "expressions.h"
#include "atoms.h"
#include "reference.h"
#include "field_access.h"


namespace gologpp {
namespace parser {


template<>
struct ExpressionParser<CompoundExpression> : public grammar<CompoundExpression *(Scope &)> {
	ExpressionParser();

	rule<CompoundExpression *(Scope &)> expression;
	rule<Reference<Variable<CompoundExpression>> *(Scope &)> var_ref;
	ReferenceParser<Fluent<CompoundExpression>> compound_fluent_ref;
	ReferenceParser<Function<CompoundExpression>> compound_function_ref;
	FieldAccessParser<CompoundExpression> compound_field_access;
};

extern rule<CompoundExpression *(Scope &)> compound_expression;


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
