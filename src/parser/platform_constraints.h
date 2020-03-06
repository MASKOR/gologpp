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

#ifndef GOLOGPP_PARSER_PLATFORM_CONSTRAINTS_H_
#define GOLOGPP_PARSER_PLATFORM_CONSTRAINTS_H_

#include "utilities.h"

#include "value.h"
#include <boost/spirit/include/qi_symbols.hpp>
#include <model/platform_constraints.h>

namespace gologpp {
namespace parser {

struct ConstraintParser
		: public grammar<vector<shared_ptr<PlatformConstraint>>()> {
	ConstraintParser();

private:
	qi::symbols<char, PlatformConstraintType> unary_constraint_type;
	qi::symbols<char, PlatformConstraintType> binary_constraint_type;
	rule<vector<std::string>()> states;
	rule<vector<ActionSpec>()> action_specs_set;
	rule<TargetSpecs()> target_specs;
	rule<ActionSpec()> action_spec;
	rule<vector<std::shared_ptr<Value>>()> args;
	rule<vector<shared_ptr<PlatformConstraint>>()> constraint;
	rule<shared_ptr<PlatformConstraint>()> unary_constraint;
	rule<shared_ptr<PlatformConstraint>()> binary_constraint;
	rule<Duration()> duration;
	rule<std::string()> text;
};

} // namespace parser
} // namespace gologpp

#endif // GOLOGPP_PARSER_PLATFORM_CONSTRAINTS_H_
