#include "compound_expression.h"
#include "list_access.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_list.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_function.hpp>


namespace gologpp {
namespace parser {


CompoundExpressionParser::CompoundExpressionParser()
: CompoundExpressionParser::base_type(expression, "compound_expression")
{
	expression = field_access(_r1)
		| compound_atom(_r1);
	expression.name("compound_expression");

	field_access = (compound_atom(_r1) >> '.' >> (r_name() % '.')) [
		_val = phoenix::bind(&nested_field_access, _1, _2),
		if_(!_val) [
			_pass = false
		]
	];
	field_access.name("nestable_compound_field_access");

	compound_atom =
		compound_literal()
		| typed_reference<Fluent>()(_r1, CompoundType::name())
		| typed_reference<Function>()(_r1, CompoundType::name())
		| list_access()(_r1, CompoundType::name())
		| var_ref(_r1)
	;
	compound_atom.name("compound_atom");

	var_ref = var_usage()(_r1, val(CompoundType::name())) [
		_val = new_<Reference<Variable>>(_1)
	];
	var_ref.name("compound_var_ref");

	GOLOGPP_DEBUG_NODES((expression)(field_access)(compound_atom)(var_ref))
}



} // namespace parser
} // namespace gologpp
