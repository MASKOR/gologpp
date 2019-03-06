#include "compound_expression.h"
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


ReferenceParser<Fluent<CompoundExpression>> compound_fluent_ref;
ReferenceParser<Function<CompoundExpression>> compound_function_ref;
FieldAccessParser<CompoundExpression> compound_field_access;
ConstantParser<CompoundExpression> compound_constant;


ExpressionParser<CompoundExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "compound_expression")
{
	expression = compound_constant | var_ref(_r1)
		| compound_fluent_ref(_r1) | compound_function_ref(_r1)
		| compound_field_access(_r1);

	var_ref = var<CompoundExpression>()(_r1) [
		_val = new_<Reference<Variable<CompoundExpression>>>(_1)
	];
}

ExpressionParser<CompoundExpression> compound_expression;


} // namespace parser
} // namespace gologpp
