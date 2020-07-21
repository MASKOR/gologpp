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

#pragma once

#include <parser/utilities.h>

#include <parser/platform/clock_formula.h>

#include <model/platform/component.h>

namespace gologpp {
namespace parser {



struct ComponentParser
: public grammar <
	platform::Component *(Scope &)
> {
	ComponentParser();

	ClockFormulaParser clock_formula;
	rule<platform::Component *(Scope &)> component;
	rule<void(platform::Component &)> clocks;
	rule<void(platform::Component &)> states;
	rule<vector<platform::AbstractTransition *>(platform::Component &)> transitions;
	rule<bool()> exog_transition_arrow;
};


} // namespace parser
} // namespace gologpp