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

#include <model/action.h>
#include <model/platform/component.h>
#include <model/reference.h>

namespace gologpp {

namespace platform {


template<>
class Reference<Action>
: public ReferenceBase<Action>
, public ChildElement
, public LanguageElement<platform::Reference<Action>, VoidType>
{
public:
	using ReferenceBase<Action>::ReferenceBase;

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<Component>
: public ZeroArityReference<platform::Component>
, public ChildElement
, public LanguageElement<Reference<Component>>
{
public:
	using ZeroArityReference<platform::Component>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<State>
: public ZeroArityReference<platform::State>
, public ChildElement
, public LanguageElement<Reference<State>>
{
public:
	using ZeroArityReference<platform::State>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<Clock>
: public ZeroArityReference<platform::Clock>
, public ChildElement
, public LanguageElement<Reference<Clock>>
{
public:
	using ZeroArityReference<platform::Clock>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////


} // namespace platform

} // namespace gologpp
