#include "types.h"
#include "list_expression.h"
#include "field_access.h"
#include "expressions.h"
#include "atoms.h"
#include "reference.h"

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


ListExpressionParser::ListExpressionParser()
: ListExpressionParser::base_type(expression, "list_expression")
{
	expression = field_access()(_r1, ListType::name())
		| list_literal()
		| typed_reference<Fluent>()(_r1, ListType::name())
		| typed_reference<Function>()(_r1, ListType::name())
		| var_ref(_r1)
	;

	var_ref = var_usage()(_r1, ListType::name()) [
		_val = new_<Reference<Variable>>(_1)
	];
}


} // namespace parser
} // namespace gologpp
