#include "compound_expression.h"
#include "atoms.h"
#include "field_access.h"
#include "reference.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {


ExpressionParser<CompoundExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "compound_expression")
, fluent_ref(new ReferenceParser<Fluent<CompoundExpression>>())
, function_ref(new ReferenceParser<Function<CompoundExpression>>())
, field_access(new FieldAccessParser<CompoundExpression>())
{
	expression = constant<CompoundExpression>() | (*field_access)(_r1) | var_ref(_r1)
		| (*fluent_ref)(_r1) | (*function_ref)(_r1);

	var_ref = var<CompoundExpression>()(_r1) [
		_val = new_<Reference<Variable<CompoundExpression>>>(_1)
	];
}


} // namespace parser
} // namespace gologpp
