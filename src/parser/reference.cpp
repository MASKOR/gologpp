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
ReferenceParser<GologT>::ReferenceParser()
: ReferenceParser::base_type(pred_ref, "reference")
{
	pred_ref = (((r_name() >> "(") > -(
		value_expression()(_r1) %  ","
	) ) > ")") [
		_val = new_<Reference<GologT>>(_1, _2),
		if_(!phoenix::bind(&Reference<GologT>::consistent, *_val)) [
			_pass = false,
			delete_(_val)
		]
	];
	pred_ref.name("reference");

	GOLOGPP_DEBUG_NODE(pred_ref)
}



template struct ReferenceParser<Action>;
template struct ReferenceParser<ExogAction>;
template struct ReferenceParser<Function>;
template struct ReferenceParser<Fluent>;





} // namespace parser
} // namespace gologpp
