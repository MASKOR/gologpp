#include "types.h"
#include "list_expression.h"
#include "atoms.h"
#include "reference.h"
#include "list_access.h"
#include "field_access.h"

#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {



rule<Expression *(Scope &)> list_atom;

rule<Expression *(Scope &)> list_expression;



void initialize_list_exprs()
{
	list_atom = {
		list_literal()
			| typed_reference<Fluent>()(_r1, ListType::name())
			| typed_reference<Function>()(_r1, ListType::name())
			| var_usage()(_r1, ListType::name()) [
				_val = new_<Reference<Variable>>(_1)
			]
		, "simple_list_expr"
	};

	list_expression = {
		mixed_field_access()(_r1, ListType::name())
		| mixed_list_access()(_r1, ListType::name())
		| list_atom(_r1)
	};
};






} // namespace parser
} // namespace gologpp
