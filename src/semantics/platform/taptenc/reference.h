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

#include <semantics/platform/taptenc/semantics.h>

#include <taptenc/src/constraints/constraints.h>
#include <taptenc/src/timed-automata/timed_automata.h>


namespace gologpp {

template<>
class Semantics<platform::Reference<platform::Clock>>
: public GeneralSemantics<platform::Reference<platform::Clock>>
{
public:
	std::shared_ptr<taptenc::Clock> compile();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::Reference<Action>>
: public GeneralSemantics<platform::Reference<Action>>
{
public:
	taptenc::ActionName compile();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::Reference<platform::State>>
: public GeneralSemantics<platform::Reference<platform::State>>
{
public:
	taptenc::State compile();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::Reference<platform::Component>>
: public GeneralSemantics<platform::Reference<platform::Component>>
{
public:
	/* No compile() method since there are no explicit references to components
	 * in taptenc. A component is simply a timed automaton and there are no references
	 * to the TA as a whole in the TA formalism. */
};


} // namespace gologpp
