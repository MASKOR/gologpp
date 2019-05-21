#ifndef GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
#define GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_

#include "utilities.h"
#include "atoms.h"
#include "reference.h"


namespace gologpp {
namespace parser {


struct CompoundExpressionParser : public grammar<Expression *(Scope &)> {
	CompoundExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> compound_atom;
	rule<Reference<Variable> *(Scope &)> var_ref;
	ReferenceParser<Fluent> compound_fluent_ref;
	ReferenceParser<Function> compound_function_ref;

	rule<Expression *(Scope &)> field_access;
};

extern rule<Expression *(Scope &)> compound_expression;


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_COMPOUND_EXPRESSION_H_
