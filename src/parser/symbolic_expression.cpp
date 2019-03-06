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


ReferenceParser<Fluent<SymbolicExpression>> symbolic_fluent_ref;
ReferenceParser<Function<SymbolicExpression>> symbolic_function_ref;
FieldAccessParser<SymbolicExpression> symbolic_field_access;
ConstantParser<SymbolicExpression> symbolic_constant;
ConstantParser<SymbolicExpression> symbolic_constant_def(true);


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


ExpressionParser<SymbolicExpression> symbolic_expression;


} // namespace parser
} // namespace gologpp
