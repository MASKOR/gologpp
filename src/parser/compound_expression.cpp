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

#include "compound_expression.h"
#include "mixed_member_access.h"
#include "value.h"
#include "variable.h"
#include "expressions.h"
#include "types.h"

#include <model/fluent.h>
#include <model/procedural.h>
#include <model/compound_expression.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/static_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>


namespace gologpp {
namespace parser {



rule<Expression *(Scope &, const Type &)> compound_atom;

rule<Expression *(Scope &, const Type &)> compound_expression;

static rule<Expression *(Scope &, const Type &), locals<shared_ptr<const CompoundType>, string>> braced_compound_expr_;


void initialize_compound_exprs()
{
	compound_atom =
		compound_value() [
			_val = _1,
			_pass = (*_val <= _r2)
		]
		| typed_reference<Fluent>()(_r1, _r2) [ _val = _1 ]
		| typed_reference<Function>()(_r1, _r2) [ _val = _1 ]
		| var_usage()(_r1, _r2) [
			_val = new_<Reference<Variable>>(_1)
		]
		, "compound_atom"
	;

	compound_expression =
		mixed_member_access()(_r1, _r2) [ _val = _1 ]
		| conditional_expression(_r1, _r2) [ _val = _1 ]
		| compound_atom(_r1, _r2) [ _val = _1 ]
		| braced_compound_expr_(_r1, _r2) [ _val = _1 ]
		, "compound_expression"
	;

	braced_compound_expr_ =
		(
			(type_identifier<CompoundType>()(_r1) [
				_a = _1,
				_pass = (*_1 <= _r2)
			]
			> '{')
			> ((
				r_name() [
					_b = _1,
					_pass = phoenix::bind(&CompoundType::has_field, *_a, _1)
				]
				> '='
				> typed_expression()(
					_r1,
					phoenix::bind(&CompoundType::field_type<Type>, *_a, _b)
				)
			) % ',') > '}'
		) [
			_val = new_<CompoundExpression>(*_a, _2)
		]
		, "braced_compound_expression"
	;

}



} // namespace parser
} // namespace gologpp
