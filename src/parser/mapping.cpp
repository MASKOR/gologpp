#include "mapping.h"
#include "expressions.h"

#include <model/action.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_attr.hpp>

#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/fusion/at.hpp>

namespace gologpp {
namespace parser {


MappingParser::MappingParser()
: MappingParser::base_type(mapping, "action_mapping")
{
	mapping = (raw_string_literal() > '{'
		> (
			r_name()
			> '='
			> value_expression()(_r1)
		) % ','
	> '}') [
		_val = new_<ActionMapping>(_1, _2)
	];
}


} // namespace parser
} // namespace gologpp
