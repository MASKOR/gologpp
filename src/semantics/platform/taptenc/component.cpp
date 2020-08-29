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

#include "component.h"
#include <model/platform/component.h>
#include <model/platform/clock_formula.h>
#include <model/logger.h>

#include <taptenc/src/constants.h>
#include <semantics/platform/taptenc/clock_formula.h>


namespace gologpp {


Semantics<platform::Clock>::Semantics(const platform::Clock &clock, AExecutionContext &context)
: GeneralSemantics<platform::Clock>(clock, context)
{
	if (element().name() != tolower(element().name()))
		throw Unsupported("Taptenc semantics require all platform model element names to be lowercase: " + element().str());
}


std::shared_ptr<taptenc::Clock> Semantics<platform::Clock>::compile()
{
	if (!ttclock_)
		ttclock_ = std::make_shared<taptenc::Clock>(uppaal_qualified_name(element()));
	return ttclock_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

taptenc::Transition Semantics<platform::Transition>::compile()
{
	std::unique_ptr<taptenc::ClockConstraint> clock_formula;
	if (element().clock_formula())
		clock_formula = element().clock_formula()->semantics().compile();
	else
		clock_formula.reset(new taptenc::TrueCC());

	taptenc::update_t resets;
	for (auto &c : element().resets())
		resets.insert((*c)->special_semantics().compile());

	return taptenc::Transition {
		element().from()->special_semantics().taptenc_name(),
		element().to()->special_semantics().taptenc_name(),
		dynamic_cast<const platform::Component &>(*element().parent()).name()
			+ "G" + element().from()->name()
			+ "T" + element().to()->name(),
		*clock_formula,
		std::move(resets),
		"" /* TODO: unused? */
	};
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

taptenc::Transition Semantics<platform::ExogTransition>::compile()
{
	std::unique_ptr<taptenc::ClockConstraint> clock_formula;
	if (element().clock_formula())
		clock_formula = element().clock_formula()->semantics().compile();
	else
		log(LogLevel::WRN) << "Exogenous transition without clock formula: " << element().str() << flush;

	taptenc::update_t resets;
	for (auto &c : element().resets())
		resets.insert((*c)->special_semantics().compile());

	return taptenc::Transition {
		element().from()->special_semantics().taptenc_name(),
		element().to()->special_semantics().taptenc_name(),
		"no_op",
		*clock_formula,
		std::move(resets),
		"" /* TODO: unused? */
	};
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////


Semantics<platform::State>::Semantics(const platform::State &state, AExecutionContext &context)
: GeneralSemantics<platform::State>(state, context)
{
	if (element().name() != tolower(element().name()))
		throw Unsupported("Taptenc semantics require all platform model element names to be lowercase: " + element().str());
}


taptenc::State Semantics<platform::State>::compile()
{
	std::unique_ptr<taptenc::ClockConstraint> clock_formula;
	if (element().clock_formula())
		clock_formula = element().clock_formula()->semantics().compile();
	else
		clock_formula.reset(new taptenc::TrueCC());

	return taptenc::State { taptenc_name(), *clock_formula };
}


std::string Semantics<platform::State>::taptenc_name()
{ return uppaal_qualified_name(element()); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////


Semantics<platform::Component>::Semantics(const platform::Component &comp, AExecutionContext &context)
: GeneralSemantics<platform::Component>(comp, context)
{
	if (element().name() != tolower(element().name()))
		throw Unsupported("Taptenc semantics require all platform model element names to be lowercase: " + element().str());
}


std::unique_ptr<taptenc::automaton> Semantics<platform::Component>::compile()
{
	platform::Component::Lock l(element().lock());

	std::vector<taptenc::State> states;
	std::vector<taptenc::Transition> transitions;

	for (auto &s : element().states()) {
		taptenc::State ttstate = s->special_semantics().compile();
		if (element().current_state() == *s)
			ttstate.initial = true;
		states.emplace_back(ttstate);
	}

	for (const unique_ptr<platform::AbstractTransition> &t : element().transitions())
		transitions.emplace_back(t->semantics<platform::AbstractTransition>().compile());

	auto rv = std::make_unique<taptenc::automaton>(
		std::move(states),
		std::move(transitions),
		element().name(),
		false /* TODO: set_trap? Wot is det ??*/
	);

	rv->clocks.emplace(new taptenc::Clock(taptenc::constants::GLOBAL_CLOCK));
	for (const shared_ptr<platform::Clock> &c : element().clocks())
		rv->clocks.emplace(c->special_semantics().compile());

	return rv;
}



} // namespace gologpp
