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

#include "semantics.h"
#include "component.h"
#include "constraint.h"
#include "clock_formula.h"

namespace gologpp {


#define GOLOGPP_DEFINE_MAKE_PLATFORM_SEMANTICS(_r, _data, T) \
unique_ptr<GeneralSemantics<ModelElement>> SemanticsFactory::make_semantics(T &element) { \
	return platform_semantics_factory_->make_semantics(element); \
}

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_PLATFORM_SEMANTICS, (), GOLOGPP_PLATFORM_ELEMENTS)


namespace platform {

#define GOLOGPP_DEFINE_DUMMY_MAKE_PLATFORM_SEMANTICS(_r, _data, T) \
unique_ptr<GeneralSemantics<ModelElement>> DummySemanticsFactory::make_semantics(T &) { \
	return nullptr; \
}

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_DUMMY_MAKE_PLATFORM_SEMANTICS, (), GOLOGPP_PLATFORM_ELEMENTS)

} // namespace platform


} // namespace gologpp

