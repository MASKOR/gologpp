#include "field_access.h"
#include "list_access.h"
#include "types.h"
#include "compound_expression.h"
#include "reference.h"

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
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {



rule<FieldAccess *(Expression *, Typename)> &single_field_access()
{
	static rule<FieldAccess *(Expression *, Typename)> rv {
		('.'  >> r_name()) [
			if_(phoenix::bind(&CompoundType::has_field_of_type,
				dynamic_cast_<const CompoundType &>(
					phoenix::bind(&AbstractLanguageElement::type, _r1)
				),
				_1,
				_r2)
			) [
				_val = new_<FieldAccess>(_r1, _1)
			].else_[
				_pass = false
			]
		]
		, "single_field_access"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<FieldAccess *(Expression *, Typename)> &deep_field_access()
{
	static rule<FieldAccess *(Expression *, Typename), locals<Expression *>> internal {
		eps [ _a = _r1 ]
		>> +(
			single_field_access()(_a, CompoundType::name()) [
				_a = _1
			]
		) >> single_field_access()(_a, _r2) [
			_val = _1
		],
		"deep_field_access"
	};
	static rule<FieldAccess *(Expression *, Typename)> rv { internal(_r1, _r2) };
	GOLOGPP_DEBUG_NODES((internal))
	return rv;
}



rule<Expression *(Scope &, Typename)> &mixed_field_access()
{
	static rule<Expression *(Scope &, Typename), locals<Expression *>> internal {
		compound_atom(_r1) [
			_a = _1
		] >> (
			(
				*(
					deep_field_access()(_a, ListType::name()) [
						_a = _1
					] >> deep_list_access()(_r1, _a, CompoundType::name()) [
						_a = _1
					]
				) >> deep_field_access()(_a, _r2) [
					_val = _1
				]
			) | +(
				deep_field_access()(_a, ListType::name()) [
					_a = _1
				] >> deep_list_access()(_r1, _a, _r2) [
					_val = _1
				]
			)
		)
		, "mixed_field_access"
	};
	GOLOGPP_DEBUG_NODE(internal)
	static rule<Expression *(Scope &, Typename)> rv { internal(_r1, _r2) };
	return rv;
}


} // namespace parser
} // namespace gologpp
