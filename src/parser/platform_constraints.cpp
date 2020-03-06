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

#include "platform_constraints.h"

#include <boost/phoenix/object/new.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include "value.h"
#include <model/platform_constraints.h>
#include <model/value.h>

namespace gologpp {
namespace parser {

ConstraintParser::ConstraintParser() : ConstraintParser::base_type(constraint) {
	unary_constraint_type.add
		("Future", PlatformConstraintType::Future)
		("NoOp", PlatformConstraintType::NoOp)
		("Past", PlatformConstraintType::Past)
		// ("Globally", PlatformConstraintType::Globally)
		// ("Historically", PlatformConstraintType::Historically)
			;
	binary_constraint_type.add("Until", PlatformConstraintType::Until)(
			"Since", PlatformConstraintType::Since);
	constraint = ("Occ" >> (unary_constraint | binary_constraint)) % "";

	unary_constraint =
			(action_specs_set >> "Then" >> unary_constraint_type >> target_specs >>
			 ";")[::boost::phoenix::new_<UnaryConstraint>(_2, _1, _3)];
	binary_constraint =
			(action_specs_set >> "Then" >> states >> binary_constraint_type >>
			 target_specs >>
			 ";")[::boost::phoenix::new_<BinaryConstraint>(_3, _1, _4, _2)];
	constraint.name("constraint");

	action_specs_set = action_spec % ',';
	action_specs_set.name("action_specs_set");

	action_spec = text >> "(" >> args >> ")";
	action_spec.name("action_spec");

	args = any_value() % ",";
	args.name("args");

	states = text % ",";
	states.name("states");

	target_specs = (states >> "Within" >> "[" >> duration >> "]");

	duration = ::boost::spirit::qi::int_ >> ',' >> ::boost::spirit::qi::int_;
	duration.name("duration");

	text = +::boost::spirit::qi::char_("a-z");
	text.name("text");
	BOOST_SPIRIT_DEBUG_NODES((constraint)(states)(action_specs_set)(target_specs)(
			action_spec)(args)(duration)(text))
}

} // namespace parser
} // namespace gologpp
