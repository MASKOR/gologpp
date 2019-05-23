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


SymbolicExpressionParser::SymbolicExpressionParser()
: SymbolicExpressionParser::base_type(expression, "symbolic_expression")
{
	expression = symbolic_constant() | var_ref(_r1) | symbolic_fluent_ref(_r1)
		| symbolic_function_ref(_r1)
		| field_access()(_r1, val(Symbol::static_name()));

	var_ref = var_usage()(_r1, val(Symbol::static_name())) [
		_val = new_<Reference<Variable>>(_1)
	];
	var_ref.name("reference_to_symbolic_variable");
}


} // namespace parser
} // namespace gologpp
