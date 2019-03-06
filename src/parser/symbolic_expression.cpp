#include "symbolic_expression.h"
#include "atoms.h"
#include "reference.h"
#include "field_access.h"

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
, fluent_ref(new ReferenceParser<Fluent<SymbolicExpression>>())
, function_ref(new ReferenceParser<Function<SymbolicExpression>>())
, field_access(new FieldAccessParser<SymbolicExpression>())
{
	expression = constant | var_ref(_r1) | (*fluent_ref)(_r1)
		| (*function_ref)(_r1) | (*field_access)(_r1);

	var_ref = var<SymbolicExpression>()(_r1) [
		_val = new_<Reference<Variable<SymbolicExpression>>>(_1)
	];
	var_ref.name("reference_to_symbolic_variable");
}


} // namespace parser
} // namespace gologpp
