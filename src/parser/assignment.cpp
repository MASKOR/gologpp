/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "assignment.h"
#include "types.h"
#include "expressions.h"
#include "mixed_member_access.h"

#include <model/list.h>
#include <model/compound.h>
#include <model/reference.h>

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
#include <boost/phoenix/object/dynamic_cast.hpp>


namespace gologpp {
namespace parser {



template<class LhsT>
AssignmentParser<LhsT>::AssignmentParser()
: AssignmentParser<LhsT>::base_type(assignment, "assignment")
{
	assignment = (
		(lhs_parser(_r1) >> "=") [
			_a = phoenix::bind(&Expression::type_ptr, *_1)
		]
		> typed_expression()(_r1, *_a)
	) [
		_val = new_<Assignment<LhsT>>(_1, _2)
	];
	assignment.name("assignment");

	init();

	//GOLOGPP_DEBUG_NODES((assignment)(lhs_parser))
}


template<>
void AssignmentParser<Reference<Fluent>>::init()
{
	static ReferenceParser<Fluent> ref_parser;
	lhs_parser = ref_parser(_r1, undefined_type());
	lhs_parser.name("fluent_lhs");
}


template<>
void AssignmentParser<FieldAccess>::init()
{
	lhs_parser = mixed_member_access()(_r1, undefined_type()) [
		_pass = (_val = dynamic_cast_<FieldAccess *>(_1))
	];
	lhs_parser.name("field_access_lhs");
}


template<>
void AssignmentParser<ListAccess>::init()
{
	lhs_parser = mixed_member_access()(_r1, undefined_type()) [
		_pass = (_val = dynamic_cast_<ListAccess *>(_1))
	];
	lhs_parser.name("list_access_lhs");
}


template
struct AssignmentParser<Reference<Fluent>>;

template
struct AssignmentParser<FieldAccess>;

template
struct AssignmentParser<ListAccess>;


} // namespace parser
} // namespace gologpp

