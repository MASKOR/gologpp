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

namespace gologpp {
namespace parser {


template<class ExprT>
DomainExpressionParser<ExprT>::DomainExpressionParser()
: DomainExpressionParser<ExprT>::base_type(domain_expr, "domain_expression")
{
	domain_expr = binary_domain_expr(_r1) | unary_domain_expr(_r1);
	domain_expr.name("domain_expression");

	unary_domain_expr =
		r_name() [ _val = *phoenix::bind(&Scope::lookup_domain<ExprT>, _r1, _1) ]
		| (lit('{') > (constant % ',') > '}') [
			_val = construct<Domain<ExprT>>(val(""), _1)
		]
	;
	unary_domain_expr.name("unary_domain_expression");

	binary_domain_expr = (unary_domain_expr(_r1) >> domain_operator >> domain_expr(_r1)) [
		_val = phoenix::bind(&domain_operation<ExprT>, _1, _2, _3)
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



template<class ExprT>
DomainDeclarationParser<ExprT>::DomainDeclarationParser()
: DomainDeclarationParser<ExprT>::base_type(domain, "domain_declaration")
{
	domain = ((type_mark<ExprT>() >> "domain") > r_name() [
		_a = _1
	] )
	> (
		lit(';') [ // forward declaration
			phoenix::bind(&Scope::declare_domain<ExprT>, _r1, _a)
		]
		| ( lit('=') > domain_expr(_r1) ) [ // definition
			phoenix::bind(&Scope::define_domain<ExprT>, _r1, _a, _1)
		]
	);
	domain.name("domain_declaration");

	GOLOGPP_DEBUG_NODE(domain);
}

template
struct DomainDeclarationParser<NumericExpression>;

template
struct DomainDeclarationParser<SymbolicExpression>;

template
struct DomainDeclarationParser<StringExpression>;



template<class ExprT>
DomainAssignmentParser<ExprT>::DomainAssignmentParser()
: DomainAssignmentParser<ExprT>::base_type(domain_assignment, type_descr<ExprT>()() + "_domain_assignment")
{
	domain_assignment = (var<ExprT, true, false>()(_r1) > "in" > domain_expr(_r1) > ';') [
		phoenix::bind(&Variable<ExprT>::set_domain_copy, _1, _2)
	];
	GOLOGPP_DEBUG_NODE(domain_assignment);
}



AnyDomainAssignmentParser::AnyDomainAssignmentParser()
: AnyDomainAssignmentParser::base_type(domain_assignment, "any_domain_assignment")
{
	domain_assignment =
		numeric_domain_assignment(_r1)
		| symbolic_domain_assignment(_r1)
		| string_domain_assignment(_r1)
	;
	domain_assignment.name("any_domain_assignment");
}



} // namespace parser
} // namespace gologpp
