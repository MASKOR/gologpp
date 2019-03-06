#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "atoms.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {


extern ReferenceParser<Fluent<SymbolicExpression>> symbolic_fluent_ref;
extern ReferenceParser<Function<SymbolicExpression>> symbolic_function_ref;
extern FieldAccessParser<SymbolicExpression> symbolic_field_access;
extern ConstantParser<SymbolicExpression> symbolic_constant;
extern ConstantParser<SymbolicExpression> symbolic_constant_def;


template<>
struct ExpressionParser<SymbolicExpression> : grammar<SymbolicExpression *(Scope &)> {
	ExpressionParser();

	rule<SymbolicExpression *(Scope &)> expression;
	rule<Reference<Variable<SymbolicExpression>> *(Scope &)> var_ref;
};


extern ExpressionParser<SymbolicExpression> symbolic_expression;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
