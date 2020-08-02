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

#include <model/platform/semantics.h>


namespace gologpp {


template<class PlatformT>
class Semantics : public GeneralSemantics<PlatformT> {
public:
	using GeneralSemantics<PlatformT>::GeneralSemantics;
};


class TaptencSemanticsFactory : public platform::SemanticsFactory {
public:
	virtual PlanTransformation *make_transformation() override;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_PLATFORM_ELEMENTS)
};


} // namespace gologpp
