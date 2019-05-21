#include "domain.h"
#include "atoms.h"
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


static DomainExpressionParser domain_expression_;

rule<Domain(Scope &, Typename)> domain_expression {
	domain_expression_(_r1, _r2)
};

DomainExpressionParser::DomainExpressionParser()
: DomainExpressionParser::base_type(domain_expr, "domain_expression")
{
	domain_expr = binary_domain_expr(_r1, _r2) | unary_domain_expr(_r1, _r2);
	domain_expr.name("domain_expression");

	typename expression::local_variable<shared_ptr<Domain>>::type p_domain;
	unary_domain_expr =
		r_name() [
			let(p_domain = phoenix::bind(&Scope::lookup_domain, _r1, _1, _r2)) [
				if_(p_domain) [
					_val = *p_domain
				].else_ [
					_pass = false
				]
			]
		]
		| (lit('{') > (constant(_r2) % ',') > '}') [
			_val = construct<Domain>(val(""), _r2, _1)
		]
	;
	unary_domain_expr.name("unary_domain_expression");

	binary_domain_expr = (unary_domain_expr(_r1, _r2) >> domain_operator >> domain_expr(_r1, _r2)) [
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



rule<void(Scope &), locals<string, Typename>> domain_decl {
	((any_type_specifier >> "domain") > r_name()) [
		_a = _1, // type name
		_b = _2  // domain name
	]
	> (
		lit(';') [ // forward declaration
			phoenix::bind(&Scope::declare_domain, _r1, _a, _b)
		]
		| ( lit('=') > domain_expression(_r1, _a) ) [ // definition
			phoenix::bind(&Scope::define_domain, _r1, _a, _b, _1)
		]
	),
	"domain_declaration"
};



rule<void(Scope &), locals<Typename>> domain_assignment {
	(
		any_var_usage(_r1) [
			_a = phoenix::bind(&Expression::type_name, _1)
		]
		> "in" > domain_expression(_r1, _a) > ';'
	) [
		phoenix::bind(&Variable::set_domain_copy, _1, _2)
	],
	"domain_assignment"
};


} // namespace parser
} // namespace gologpp
