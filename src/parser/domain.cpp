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

#include "domain.h"
#include "variable.h"
#include "value.h"
#include "types.h"

#include <model/variable.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/scope/let.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/scope/local_variable.hpp>

namespace gologpp {
namespace parser {



rule<Domain(Scope &, const Type &)> &domain_expression()
{
	static DomainExpressionParser domain_expression_;
	static rule<Domain(Scope &, const Type &)> rv {
		domain_expression_(_r1, _r2, false)
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
};



DomainExpressionParser::DomainExpressionParser()
: DomainExpressionParser::base_type(domain_expr, "domain_expression")
{
	domain_expr = binary_domain_expr(_r1, _r2, _r3) | unary_domain_expr(_r1, _r2, _r3);
	domain_expr.name("domain_expression");

	unary_domain_expr =
		r_name() [
			_a = phoenix::bind(&Scope::lookup_domain, _r1, _1, _r2),
			if_(_a) [
				_val = *_a
			].else_ [
				_pass = false
			]
		]
		| (lit('{') > (value()(_r2, _r3) % ',') > '}') [
			_val = construct<Domain>(val(""), _r2, _1)
		]
	;
	unary_domain_expr.name("unary_domain_expression");

	binary_domain_expr = (unary_domain_expr(_r1, _r2, _r3) >> domain_operator >> domain_expr(_r1, _r2, _r3)) [
		_val = phoenix::bind(&domain_operation, _1, _2, _3)
	];
	binary_domain_expr.name("binary_domain_expression");

	domain_operator =
		lit('|') [ _val = val(DomainOperator::UNION) ]
		| lit('\\') [ _val = val(DomainOperator::DIFFERENCE) ]
		| lit('&') [ _val = val(DomainOperator::INTERSECTION) ]
	;
	domain_operator.name("domain_operator");

	GOLOGPP_DEBUG_NODES((domain_expr)(unary_domain_expr)(binary_domain_expr))
}



rule<void(Scope &), locals<string, shared_ptr<const Type>>> &domain_decl()
{
	static DomainExpressionParser domain_expr_;
	static rule<void(Scope &), locals<string, shared_ptr<const Type>>> rv {
		((type_identifier<Type>()(_r1) >> "domain") > r_name()) [
			_b = _1, // type
			_a = _2  // domain name
		]
		> (
			lit(';') [ // forward declaration
				phoenix::bind(&Scope::declare_domain, _r1, _a, *_b)
			]
			| ( lit('=') > domain_expr_(_r1, *_b, true) ) [ // definition
				phoenix::bind(&Scope::define_domain, _r1, _a, *_b, _1)
			]
		),
		"domain_declaration"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



} // namespace parser
} // namespace gologpp
