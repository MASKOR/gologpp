#include "compound_expression.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_list.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_function.hpp>


namespace gologpp {
namespace parser {


ExpressionParser<CompoundExpression> &compound_expression_()
{
	static ExpressionParser<CompoundExpression> rv;
	return rv;
}

rule<CompoundExpression *(Scope &)> compound_expression = compound_expression_()(_r1);

ExpressionParser<CompoundExpression>::ExpressionParser()
: ExpressionParser::base_type(expression, "compound_expression")
{
	expression = field_access(_r1)
		| compound_atom(_r1);
	expression.name("compound_expression");

	field_access = (compound_atom(_r1) >> '.' >> (field_name % '.')) [
		_val = phoenix::bind(&nested_field_access, _1, _2)
	];
	field_access.name("nestable_compound_field_access");

	field_name = type_mark<CompoundExpression>() >> r_name();
	field_name.name("compound_field_name");

	compound_atom =
		compound_constant | var_ref(_r1)
		| compound_fluent_ref(_r1)
		| compound_function_ref(_r1)
	;
	compound_atom.name("compound_atom");

	var_ref = var<CompoundExpression>()(_r1) [
		_val = new_<Reference<Variable<CompoundExpression>>>(_1)
	];
	var_ref.name("compound_var_ref");

	GOLOGPP_DEBUG_NODES((expression)(field_access)(field_name)(compound_atom)(var_ref));
}



} // namespace parser
} // namespace gologpp
