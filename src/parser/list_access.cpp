#include "list_access.h"
#include "list_expression.h"
#include "field_access.h"
#include "expressions.h"

#include <model/procedural.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_plus.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>


namespace gologpp {
namespace parser {


rule<ListAccess *(Scope &, Expression *, Typename)> &single_list_access()
{
	static rule<ListAccess *(Scope &, Expression *, Typename)> rv {
		('[' >> numeric_expression(_r1) >> ']') [
			if_(phoenix::bind(&ListType::element_type,
				dynamic_cast_<const ListType &>(
					phoenix::bind(&AbstractLanguageElement::type, _r2)
				)) == _r3
			) [
				_val = new_<ListAccess>(_r2, _1)
			].else_[
				_pass = false
			]
		]
		, "single_list_access"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<ListAccess *(Scope &, Expression *, Typename)> &deep_list_access()
{
	static rule<ListAccess *(Scope &, Expression *, Typename), locals<Expression *>> internal {
		eps [ _a = _r2 ]
		>> +(
			single_list_access()(_r1, _r2, ListType::name()) [
				_a = _1
			]
		) >> single_list_access()(_r1, _a, _r3) [
			_val = _1
		]
		, "deep_list_access"
	};
	GOLOGPP_DEBUG_NODE(internal)
	static rule<ListAccess *(Scope &, Expression *, Typename)> rv { internal(_r1, _r2, _r3) };
	return rv;
}


rule<Expression *(Scope &, Typename)> &mixed_list_access()
{
	static rule<Expression *(Scope &, Typename), locals<Expression *>> internal {
		list_atom(_r1) [
			_a = _1
		] >> (
			*(
				deep_list_access()(_r1, _a, CompoundType::name()) [
					_a = _1
				] >> deep_field_access()(_a, ListType::name()) [
					_a = _1
				]
			) >> deep_list_access()(_r1, _a, _r2) [
				_val = _1
			]
		) | +(
			deep_list_access()(_r1, _a, CompoundType::name()) [
				_a = _1
			] >> deep_field_access()(_a, _r2) [
				_a = _1
			]
		)
		, "mixed_list_access"
	};
	GOLOGPP_DEBUG_NODE(internal)
	static rule<Expression *(Scope &, Typename)> rv { internal(_r1, _r2) };
	return rv;
}



} // namespace parser
} // namespace gologpp


