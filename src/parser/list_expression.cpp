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

#include "types.h"
#include "list_expression.h"
#include "variable.h"
#include "value.h"
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
		list_literal() [ _val = _1 ]
			| typed_reference<Fluent>()(_r1, ListType::name()) [ _val = _1 ]
			| typed_reference<Function>()(_r1, ListType::name()) [ _val = _1 ]
			| var_usage()(_r1, ListType::name()) [
				_val = new_<Reference<Variable>>(_1)
			]
		, "simple_list_expr"
	};

	list_expression = {
		mixed_field_access()(_r1, ListType::name()) [ _val = _1 ]
			| mixed_list_access()(_r1, ListType::name()) [ _val = _1 ]
			| list_atom(_r1) [ _val = _1 ]
		, "list_expression"
	};

	GOLOGPP_DEBUG_NODES(
		(list_atom)
		(list_expression)
	)
};






} // namespace parser
} // namespace gologpp
