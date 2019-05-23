#include "field_access.h"
#include "types.h"
#include "compound_expression.h"
#include "reference.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {


rule<FieldAccess *(Scope &, Typename)> &field_access()
{
	static CompoundExpressionParser compound_expression_;
	static rule<FieldAccess *(Scope &, Typename), locals<const CompoundType *>> internal { (
		(compound_expression_(_r1) >> lit('.')) > r_name()) [
			_a = dynamic_cast_<const CompoundType *>(
				&phoenix::bind(&AbstractLanguageElement::type, _1)
			),
			if_(phoenix::bind(&CompoundType::has_field_of_type, *_a, _2, _r2)) [
				_val = new_<FieldAccess>(_1, _2)
			].else_ [
				_pass = false
			]
		],
		"field_access_checked"
	};
	static rule<FieldAccess *(Scope &, Typename)> rv {
		internal(_r1, _r2),
		"field_access"
	};
	return rv;
}


rule<FieldAccess *(Scope &)> &any_field_access()
{
	static CompoundExpressionParser compound_expression_;
	static rule<FieldAccess *(Scope &)> rv {
		((compound_expression_(_r1) >> '.' ) > r_name()) [
			_val = new_<FieldAccess>(_1, _2)
		],
		"any_field_access"
	};
	return rv;
}


} // namespace parser
} // namespace gologpp
