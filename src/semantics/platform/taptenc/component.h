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

#include <model/platform/component.h>
#include <model/semantics.h>

#include <semantics/platform/taptenc/semantics.h>

#include <taptenc/src/timed-automata/timed_automata.h>

namespace gologpp {


template<>
class Semantics<platform::Clock>
: public GeneralSemantics<platform::Clock>
{
public:
	std::shared_ptr<taptenc::Clock> compile();

private:
	std::shared_ptr<taptenc::Clock> ttclock_;
};


// Only endogenous transitions have TaptencSemantics.
// Exogenous ones are disregarded in the plan transformation.
template<>
class Semantics<platform::Transition>
: public GeneralSemantics<platform::Transition>
{
public:
	taptenc::Transition compile();
};


template<>
class Semantics<platform::State>
: public GeneralSemantics<platform::State>
{
public:
	taptenc::State compile();
};


template<>
class Semantics<platform::Component>
: public GeneralSemantics<platform::Component>
{
public:
	std::unique_ptr<taptenc::automaton> compile();
};


} // namespace gologpp
