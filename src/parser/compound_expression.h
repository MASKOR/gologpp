#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "expressions.h"
#include "atoms.h"
#include "reference.h"


namespace gologpp {
namespace parser {


template<>
struct ExpressionParser<CompoundExpression> : public grammar<CompoundExpression *(Scope &)> {
	ExpressionParser();

	rule<CompoundExpression *(Scope &)> expression;
	rule<CompoundExpression *(Scope &)> compound_atom;
	rule<Reference<Variable<CompoundExpression>> *(Scope &)> var_ref;
	ReferenceParser<Fluent<CompoundExpression>> compound_fluent_ref;
	ReferenceParser<Function<CompoundExpression>> compound_function_ref;

	rule<CompoundExpression *(Scope &)> field_access;
	rule<string()> field_name;
};

extern rule<CompoundExpression *(Scope &)> compound_expression;


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
