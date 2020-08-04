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

#include <model/platform/clock_formula.h>
#include <model/platform/component.h>
#include <model/platform/reference.h>
#include <model/platform/constraint.h>
#include <model/platform/switch_state_action.h>

#include "semantics.h"
#include "transformation.h"


namespace gologpp {


string tolower(const string &s)
{
	string rv(s);
	std::transform(
		s.begin(), s.end(),
		rv.begin(),
		[] (const string::value_type &c) {
			return std::tolower(c);
		}
	);
	return s;
}


BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS, TaptencSemanticsFactory, GOLOGPP_PLATFORM_ELEMENTS)

PlanTransformation *TaptencSemanticsFactory::make_transformation()
{ return new TaptencTransformation(); }



} // namespace gologpp
