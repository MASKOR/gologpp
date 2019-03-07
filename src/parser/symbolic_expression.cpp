#include "symbolic_expression.h"
#include "atoms.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {


ExpressionParser<SymbolicExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "symbolic_expression")
{
	expression = symbolic_constant | var_ref(_r1) | symbolic_fluent_ref(_r1)
		| symbolic_function_ref(_r1) | symbolic_field_access(_r1);

	var_ref = var<SymbolicExpression>()(_r1) [
		_val = new_<Reference<Variable<SymbolicExpression>>>(_1)
	];
	var_ref.name("reference_to_symbolic_variable");
}


ExpressionParser<SymbolicExpression> &symbolic_expression_()
{
	static ExpressionParser<SymbolicExpression> rv;
	return rv;
}

rule<SymbolicExpression *(Scope &)> symbolic_expression = symbolic_expression_()(_r1);


} // namespace parser
} // namespace gologpp
