#include "compound_expression.h"
#include "list_access.h"
#include "field_access.h"
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



rule<Expression *(Scope &)> compound_atom;

rule<Expression *(Scope &)> compound_expression;



void initialize_compound_exprs()
{
	compound_atom =
		compound_literal()
		| typed_reference<Fluent>()(_r1, CompoundType::name())
		| typed_reference<Function>()(_r1, CompoundType::name())
		| var_usage()(_r1, val(CompoundType::name())) [
			_val = new_<Reference<Variable>>(_1)
		]
		, "compound_atom"
	;

	compound_expression =
		mixed_field_access()(_r1, CompoundType::name())
		| mixed_list_access()(_r1, CompoundType::name())
		| compound_atom(_r1)
		, "compound_expression"
	;
}



} // namespace parser
} // namespace gologpp
