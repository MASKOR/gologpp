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
#include <parser/value.h>

#include <model/platform/reference.h>

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
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

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


namespace gologpp {
namespace parser {

template<class PlatformT>
platform::Reference<PlatformT> *get_ref(const Scope &scope, const string &name)
{
	shared_ptr<PlatformT> tgt = scope.lookup_identifier<PlatformT>(name);
	if (tgt)
		return new platform::Reference<PlatformT>(tgt);
	else
		return nullptr;
}


template<class PlatformT>
rule<platform::Reference<PlatformT> *(Scope &)> &platform_ref() {
	static rule<platform::Reference<PlatformT> *(Scope &)> rv {
		r_name() [
			_val = phoenix::bind(&get_ref<PlatformT>, _r1, _1),
			_pass = !!_val
		]
	};
	rv.name(debug_name<PlatformT>() + "_ref");
	return rv;
};

template
rule<platform::Reference<platform::Clock> *(Scope &)> &platform_ref();

template
rule<platform::Reference<platform::State> *(Scope &)> &platform_ref();

template
rule<platform::Reference<platform::Component> *(Scope &)> &platform_ref();


template<class GologT>
platform::Reference<GologT> *get_platform_ref(
	const Type &type,
	const string &name,
	const boost::optional<vector<Value *>> &args
) {
	shared_ptr<GologT> g = global_scope().lookup_global<GologT>(name);

	if (g && g->type() <= type)
		return new platform::Reference<GologT>(name, args);
	else
		return nullptr;
}


template<class GologT>
PlatformRefParser<GologT>::PlatformRefParser()
: PlatformRefParser::base_type(start, "grounding")
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
		_val = phoenix::bind(&get_platform_ref<GologT>, _r2, _1, _2),
		if_(!_val || !phoenix::bind(&ReferenceBase<GologT, Value>::consistent, *_val)) [
			_pass = false,
			delete_(_val)
		]
	];
	pred_ref.name("reference");

	ref_args =
		eps [ _a = _r2 ]
		> repeat(phoenix::bind(&TypeList::size, _r2)) [
			value_or_wildcard(phoenix::bind(pop_front, _a))
			> lazy(phoenix::bind(conditional_comma, _a))
		] [ _val = _1 ]
	;

	value_or_wildcard = lit('*') [ _val = new_<Value>() ]
		| value()(_r1, false) [ _val = _1 ];
	value_or_wildcard.name("value_or_wildcard");

	GOLOGPP_DEBUG_NODE(pred_ref)
}

template struct PlatformRefParser<Action>;



}
}
