#include "symbolic_expression.h"
#include "atoms.h"

#include <model/fluent.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/phoenix/object/new.hpp>

namespace gologpp {
namespace parser {


ExpressionParser<SymbolicExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "symbolic_expression")
{
	expression = var_ref(_r1) | fluent_ref(_r1) | constant<SymbolicExpression>();

	var_ref = var<SymbolicExpression>()(_r1) [
		_val = new_<Reference<Variable<SymbolicExpression>>>(_1)
	];
	var_ref.name("reference_to_symbolic_variable");
}


}
}
