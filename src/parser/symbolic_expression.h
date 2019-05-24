#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "utilities.h"
#include "atoms.h"
#include "reference.h"
#include "field_access.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {


struct SymbolicExpressionParser : grammar<Expression *(Scope &)> {
	SymbolicExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Reference<Variable> *(Scope &)> var_ref;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
