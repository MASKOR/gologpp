#ifndef GOLOGPP_PARSER_STRING_EXPRESSION_H_
#define GOLOGPP_PARSER_STRING_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "fluent.h"
#include "atoms.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {

template<class> struct ReferenceParser;
template<class> struct FieldAccessParser;

template<>
struct ExpressionParser<StringExpression> : grammar<StringExpression *(Scope &)> {
	ExpressionParser();

	rule<StringExpression *(Scope &)> expression;
	rule<StringExpression *(Scope &)> unary_expr;
	unique_ptr<ReferenceParser<Fluent<StringExpression>>> fluent_ref;
	unique_ptr<ReferenceParser<Function<StringExpression>>> function_ref;
	rule<Reference<Variable<StringExpression>> *(Scope &)> var_ref;
	unique_ptr<FieldAccessParser<StringExpression>> field_access;
	rule<StringExpression *(Scope &)> concatenation;
	ConstantParser<StringExpression> string_constant;
};


} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_STRING_EXPRESSION_H_
