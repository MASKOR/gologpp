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
