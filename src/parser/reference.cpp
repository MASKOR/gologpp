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

#include "field_access.h"
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
#include <boost/spirit/include/qi_lazy.hpp>

#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/core/value.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/reference.h>

namespace gologpp {
namespace parser {


template<class GologT>
Reference<GologT> *get_ref(const string &name, const boost::optional<vector<Expression *>> &args)
{
	if (global_scope().lookup_global<GologT>(name, static_cast<arity_t>(args.get_value_or({}).size())))
		return new Reference<GologT>(name, args);
	else
		return nullptr;
}


template<class GologT>
ReferenceParser<GologT>::ReferenceParser()
: ReferenceParser::base_type(pred_ref, "reference")
{
	pred_ref = (((r_name() >> "(") >> -(
		value_expression()(_r1) %  ","
	) ) >> ")") [
			_val = phoenix::bind(&get_ref<GologT>, _1, _2),
			if_(!_val || !phoenix::bind(&ReferenceBase<GologT, Expression>::consistent, *_val)) [
				_pass = false,
				delete_(_val)
			]
	];
	pred_ref.name("reference");

	//GOLOGPP_DEBUG_NODE(pred_ref)
}



template struct ReferenceParser<Action>;
template struct ReferenceParser<ExogAction>;
template struct ReferenceParser<Function>;
template struct ReferenceParser<Fluent>;



template<class GologT>
Reference<GologT> *get_typed_ref(
	const string &type,
	const string &name,
	const boost::optional<vector<Expression *>> &args
) {
	shared_ptr<GologT> g = global_scope().lookup_global<GologT>(
		name,
		static_cast<arity_t>(args.get_value_or({}).size())
	);

	if (g && g->type() == type)
		return new Reference<GologT>(name, args);
	else
		return nullptr;
}



template<class GologT>
rule<Reference<GologT> *(Scope &, Typename)> &typed_reference()
{
	static rule<Reference<GologT> *(Scope &, Typename)> rv {
		(((r_name() >> "(") >> -(
			value_expression()(_r1) %  ","
		) ) >> ")") [
			_val = phoenix::bind(&get_typed_ref<GologT>, _r2, _1, _2),
			if_(!_val || !phoenix::bind(&ReferenceBase<GologT, Expression>::consistent, *_val)) [
				_pass = false,
				delete_(_val)
			]
		],
		"typed_reference"
	};
	//GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


template
rule<Reference<Fluent> *(Scope &, Typename)> &typed_reference();

template
rule<Reference<Function> *(Scope &, Typename)> &typed_reference();



} // namespace parser
} // namespace gologpp
