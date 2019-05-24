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
	assignment = (
		(lhs_parser(_r1) >> "=") [
			_a = phoenix::bind(&Expression::type_name, *_1)
		]
		> typed_expression()(_r1, _a)
	) [
		_val = new_<Assignment<LhsT>>(_1, _2)
	];
	assignment.name("assignment");

	init();

	GOLOGPP_DEBUG_NODES((assignment)(lhs_parser))
}


template<>
void AssignmentParser<Reference<Fluent>>::init()
{
	static ReferenceParser<Fluent> ref_parser;
	lhs_parser = ref_parser(_r1);
	lhs_parser.name("fluent_lhs");
}


template<>
void AssignmentParser<FieldAccess>::init()
{
	lhs_parser = field_access()(_r1, val(""));
	lhs_parser.name("field_access_lhs");
}


template
struct AssignmentParser<Reference<Fluent>>;

template
struct AssignmentParser<FieldAccess>;


} // namespace parser
} // namespace gologpp

