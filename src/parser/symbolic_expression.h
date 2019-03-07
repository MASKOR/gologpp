#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "atoms.h"
#include "reference.h"
#include "field_access.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {




template<>
struct ExpressionParser<SymbolicExpression> : grammar<SymbolicExpression *(Scope &)> {
	ExpressionParser();

	rule<SymbolicExpression *(Scope &)> expression;
	rule<Reference<Variable<SymbolicExpression>> *(Scope &)> var_ref;
	ReferenceParser<Fluent<SymbolicExpression>> symbolic_fluent_ref;
	ReferenceParser<Function<SymbolicExpression>> symbolic_function_ref;
	FieldAccessParser<SymbolicExpression> symbolic_field_access;
};


extern rule<SymbolicExpression *(Scope &)> symbolic_expression;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
