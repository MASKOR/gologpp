#ifndef GOLOGPP_PARSER_STRING_EXPRESSION_H_
#define GOLOGPP_PARSER_STRING_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "fluent.h"
#include "atoms.h"
#include "reference.h"
#include "field_access.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {


template<>
struct ExpressionParser<StringExpression> : grammar<StringExpression *(Scope &)> {
	ExpressionParser();

	rule<StringExpression *(Scope &)> expression;
	rule<StringExpression *(Scope &)> unary_expr;
	rule<Reference<Variable<StringExpression>> *(Scope &)> var_ref;
	rule<StringExpression *(Scope &)> concatenation;
	ReferenceParser<Fluent<StringExpression>> string_fluent_ref;
	ReferenceParser<Function<StringExpression>> string_function_ref;
	FieldAccessParser<StringExpression> string_field_access;
};


extern rule<StringExpression *(Scope &)> string_expression;


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_STRING_EXPRESSION_H_
