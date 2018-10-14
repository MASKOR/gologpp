#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "fluent.h"
#include "reference.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {

template<>
struct ExpressionParser<SymbolicExpression> : grammar<SymbolicExpression *(Scope &)> {
	ExpressionParser();

	rule<SymbolicExpression *(Scope &)> expression;
	ReferenceParser<Fluent<SymbolicExpression>> fluent_ref;
	rule<Reference<Variable<SymbolicExpression>> *(Scope &)> var_ref;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
