#include "field_access.h"
#include "assignment.h"
#include "atoms.h"
#include "arithmetic.h"
#include "formula.h"
#include "types.h"
#include "expressions.h"

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/scope/local_variable.hpp>


namespace gologpp {
namespace parser {



template<class LhsT>
AssignmentParser<LhsT>::AssignmentParser()
: AssignmentParser<LhsT>::base_type(assignment, "assignment")
{
	typename expression::local_variable<Typename>::type lhs_type;
	assignment = (
		(lhs_parser(_r1) >> "=") [
			lhs_type = phoenix::bind(&Expression::type_name, _1)
		]
		> typed_expression(_r1, lhs_type)
	) [
		_val = new_<Assignment<LhsT>>(_1, _2)
	];
	assignment.name("assignment");
}


template<>
void AssignmentParser<Reference<Fluent>>::init()
{
	lhs_parser = ReferenceParser<Fluent>()(_r1);
}


template<>
void AssignmentParser<FieldAccess>::init()
{
	lhs_parser = field_access(_r1, val(""));
}


} // namespace parser
} // namespace gologpp

