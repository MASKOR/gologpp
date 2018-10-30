#ifndef GOLOGPP_PARSER_STRING_EXPRESSION_H_
#define GOLOGPP_PARSER_STRING_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "fluent.h"
#include "reference.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {

template<>
struct ExpressionParser<StringExpression> : grammar<StringExpression *(Scope &)> {
	ExpressionParser();

	rule<StringExpression *(Scope &)> expression;
	ReferenceParser<Fluent<StringExpression>> fluent_ref;
	ReferenceParser<Function<StringExpression>> function_ref;
	rule<Reference<Variable<StringExpression>> *(Scope &)> var_ref;
	rule<StringExpression *(Scope &)> concatenation;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_STRING_EXPRESSION_H_
