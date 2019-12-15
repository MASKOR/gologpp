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

#include "effect_axiom.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_lit.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>

#include <model/effect_axiom.h>

#include "reference.h"
#include "mixed_member_access.h"
#include "expressions.h"
#include "types.h"


namespace gologpp {
namespace parser {


template<class LhsT>
EffectParser<LhsT>::EffectParser()
: EffectParser<LhsT>::base_type(effect, "effect_axiom")
{
	effect = (
		(-(lit("if") > '(' > condition(_r1) > ')')
		>> lhs(_r1) [
			_a = phoenix::bind(&Expression::type_ptr, *_1)
		])
		> '=' > typed_expression()(_r1, *_a)
	) [
		_val = new_<EffectAxiom<LhsT>>(),
		phoenix::bind(&EffectAxiom<LhsT>::define, *_val, at_c<0>(_1), at_c<1>(_1), _2)
	];
	effect.name("effect_axiom");
	on_error<rethrow>(effect, delete_(_val));

	init();

	GOLOGPP_DEBUG_NODES((effect)(lhs))
}


template<>
void EffectParser<Reference<Fluent>>::init()
{
	static ReferenceParser<Fluent> ref;
	lhs = ref(_r1);
	lhs.name("fluent_effect_lhs");
}


template<>
void EffectParser<FieldAccess>::init()
{
	lhs = mixed_member_access()(_r1, undefined_type()) [
		_pass = (_val = dynamic_cast_<FieldAccess *>(_1))
	];
	lhs.name("field_access_effect_lhs");
}


template<>
void EffectParser<ListAccess>::init()
{
	lhs = mixed_member_access()(_r1, undefined_type()) [
		_pass = (_val = dynamic_cast_<ListAccess *>(_1))
	];
	lhs.name("list_access_effect_lhs");
}

template
struct EffectParser<Reference<Fluent>>;

template
struct EffectParser<FieldAccess>;

template
struct EffectParser<ListAccess>;


}
}
