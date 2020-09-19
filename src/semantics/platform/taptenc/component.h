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
	Semantics(const platform::Clock &clock, AExecutionController &context);

	std::shared_ptr<taptenc::Clock> compile();

private:
	std::shared_ptr<taptenc::Clock> ttclock_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::AbstractTransition>
{
public:
	virtual taptenc::Transition compile() = 0;
};

template<>
class Semantics<platform::Transition>
: public GeneralSemantics<platform::Transition>
, public Semantics<platform::AbstractTransition>
{
public:
	using GeneralSemantics<platform::Transition>::GeneralSemantics;

	virtual taptenc::Transition compile() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::ExogTransition>
: public GeneralSemantics<platform::ExogTransition>
, public Semantics<platform::AbstractTransition>
{
public:
	using GeneralSemantics<platform::ExogTransition>::GeneralSemantics;

	virtual taptenc::Transition compile() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::State>
: public GeneralSemantics<platform::State>
{
public:
	Semantics(const platform::State &clock, AExecutionController &context);

	taptenc::State compile();
	std::string taptenc_name();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<platform::Component>
: public GeneralSemantics<platform::Component>
{
public:
	Semantics(const platform::Component &clock, AExecutionController &context);

	std::unique_ptr<taptenc::automaton> compile();
};


} // namespace gologpp
