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

#include "list_access.h"
#include "list_expression.h"
#include "field_access.h"
#include "expressions.h"

#include <model/procedural.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_alternative.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>


namespace gologpp {
namespace parser {


static Expression *build_mixed_list_access(
	Expression *subj,
	vector<fusion_wtf_vector<vector<Expression *>, vector<string>>> members,
	vector<Expression *> indices
) {
	Expression *rv = subj;
	for (auto &pair : members) {
		for (Expression *idx : at_c<0>(pair))
			rv = new ListAccess(rv, idx);
		for (const string &field : at_c<1>(pair))
			rv = new FieldAccess(rv, field);
	}
	for (Expression *idx : indices)
		rv = new ListAccess(rv, idx);

	return rv;
}


rule<Expression *(Scope &)> list_access {
	lit('[') >> numeric_expression(_r1) >> ']'
	, "list_access"
};


rule<Expression *(Scope &, Typename)> &mixed_list_access()
{
	static rule<Expression *(Scope &, Typename)> rv {
		(
			list_atom(_r1) >> *(
				+list_access(_r1)
				>> +field_access
			) >> +list_access(_r1)
		) [
			_val = phoenix::bind(&build_mixed_list_access, _1, _2, _3),
			if_(phoenix::bind(&AbstractLanguageElement::type, _val) != _r2) [
				_pass = false
			]
		]
		| (
			list_atom(_r1) >> +(
				+list_access(_r1)
				>> +field_access
			)
		) [
			_val = phoenix::bind(&build_mixed_list_access, _1, _2, val(vector<Expression *>{})),
			if_(phoenix::bind(&AbstractLanguageElement::type, _val) != _r2) [
				_pass = false
			]
		]
		, "mixed_list_access"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



} // namespace parser
} // namespace gologpp


