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

#include <semantics/platform/taptenc/clock_formula.h>
#include <semantics/platform/taptenc/component.h>
#include <semantics/platform/taptenc/constraint.h>
#include <semantics/platform/taptenc/reference.h>

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



namespace platform {

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS, TaptencSemanticsFactory, GOLOGPP_PLATFORM_ELEMENTS)

unique_ptr<SemanticsFactory> SemanticsFactory::construct()
{ return unique_ptr<SemanticsFactory>(new TaptencSemanticsFactory()); }

PlanTransformation *TaptencSemanticsFactory::make_transformation()
{ return new TaptencTransformation(); }

} // namespace platform


} // namespace gologpp
