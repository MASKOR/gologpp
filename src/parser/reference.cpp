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

#include "reference.h"
#include "utilities.h"
#include "expressions.h"

#include <model/action.h>
#include <model/procedural.h>
#include <model/fluent.h>

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_lazy.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/core/value.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/scope/local_variable.hpp>

#include <model/reference.h>

namespace gologpp {
namespace parser {



template<class GologT>
Reference<GologT> *get_ref(
	const Type &type,
	const string &name,
	const boost::optional<vector<Expression *>> &args
) {
	shared_ptr<GologT> g = global_scope().lookup_global<GologT>(name);

	if (g && g->type() <= type)
		return new Reference<GologT>(name, args);
	else
		return nullptr;
}


rule<void()> conditional_comma(const TypeList &tl)
{
	static rule<void()> comma { ',' };
	static rule<void()> nothing { eps };
	return tl.empty() ? nothing : comma;
}


static std::function<const Type &(TypeList &)> pop_front {
	[] (TypeList &tl) -> const Type & {
		const Type &rv = tl.front();
		tl.pop_front();
		return rv;
	}
};


template<class GologT>
ReferenceParser<GologT>::ReferenceParser()
: ReferenceParser::base_type(start, "reference")
{
	start = pred_ref(_r1, _r2);

	pred_ref = (
		(r_name() >> "(") [
			_pass = phoenix::bind(&Scope::exists_global, _r1, _1),
			if_(_pass) [
				_a = phoenix::bind(&Scope::param_types, _r1, _1)
			]
		]
		> ref_args(_r1, _a)
		> ")"
	) [
		_val = phoenix::bind(&get_ref<GologT>, _r2, _1, _2),
		if_(!_val || !phoenix::bind(&ReferenceBase<GologT, Expression>::consistent, *_val)) [
			_pass = false,
			delete_(_val)
		]
	];
	pred_ref.name("reference");

	ref_args =
		eps [ _a = _r2 ]
		> repeat(phoenix::bind(&TypeList::size, _r2)) [
			typed_expression()(_r1, phoenix::bind(pop_front, _a))
			> lazy(phoenix::bind(conditional_comma, _a))
		] [ _val = _1 ]
	;

	GOLOGPP_DEBUG_NODE(pred_ref)
}



template struct ReferenceParser<Action>;
template struct ReferenceParser<ExogAction>;
template struct ReferenceParser<Function>;
template struct ReferenceParser<Procedure>;
template struct ReferenceParser<Fluent>;




} // namespace parser
} // namespace gologpp
