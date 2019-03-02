#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "expressions.h"


namespace gologpp {
namespace parser {


template<>
struct ExpressionParser<CompoundExpression> : public grammar<CompoundExpression *(Scope &)> {
	ExpressionParser();

	rule<CompoundExpression *(Scope &)> expression;
	rule<Reference<Variable<CompoundExpression>> *(Scope &)> var_ref;
	unique_ptr<ReferenceParser<Fluent<CompoundExpression>>> fluent_ref;
	unique_ptr<ReferenceParser<Function<CompoundExpression>>> function_ref;
	unique_ptr<FieldAccessParser<CompoundExpression>> field_access;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
