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

#include "mixed_member_access.h"
#include "types.h"
#include "compound.h"
#include "expressions.h"
#include "list.h"

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_plus.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/list.h>
#include <model/compound.h>


namespace gologpp {
namespace parser {


static rule<string()> field_access {
	lit('.') >> r_name()
	, "field_access"
};

static rule<Expression *(Scope &)> list_access {
	lit('[') >> numeric_expression(_r1) >> ']'
	, "list_access"
};


rule<Expression *(Scope &, const Type &)> &mixed_member_access()
{
	static rule<Expression *(Scope &, const Type &), locals<Expression *>> rv_local {
		(
			(compound_atom(_r1, undefined_type()) | list_atom(_r1, undefined_type())) [ _a = _1 ]
			>> +(
				field_access [ _a = new_<FieldAccess>(_a, _1) ]
				| list_access(_r1) [ _a = new_<ListAccess>(_a, _1) ]
			)
		) [
			_val = _a,
			_pass = (phoenix::bind(&AbstractLanguageElement::type, *_val) <= _r2)
		]
		, "mixed_member_access"
	};
	//GOLOGPP_DEBUG_NODE(rv_local)

	static rule<Expression *(Scope &, const Type &)> rv { rv_local(_r1, _r2) };
	return rv;
}


} // namespace parser
} // namespace gologpp
