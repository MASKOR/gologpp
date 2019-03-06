#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "expressions.h"
#include "utilities.h"
#include "atoms.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {


template<>
struct ExpressionParser<SymbolicExpression> : grammar<SymbolicExpression *(Scope &)> {
	ExpressionParser();

	rule<SymbolicExpression *(Scope &)> expression;
	unique_ptr<ReferenceParser<Fluent<SymbolicExpression>>> fluent_ref;
	unique_ptr<ReferenceParser<Function<SymbolicExpression>>> function_ref;
	unique_ptr<FieldAccessParser<SymbolicExpression>> field_access;
	rule<Reference<Variable<SymbolicExpression>> *(Scope &)> var_ref;
	ConstantParser<SymbolicExpression> constant;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
