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

#include <semantics/platform/taptenc/clock_formula.h>


namespace gologpp {


std::shared_ptr<taptenc::Clock> Semantics<platform::Clock>::compile()
{
	if (!ttclock_)
		ttclock_ = std::make_shared<taptenc::Clock>(element().name());
	return ttclock_;
}



taptenc::Transition Semantics<platform::Transition>::compile()
{
	std::unique_ptr<taptenc::ClockConstraint> clock_formula;
	if (element().clock_formula())
		clock_formula = dynamic_cast<ClockFormulaSemantics &>(element().clock_formula()->semantics()).compile();
	else
		clock_formula.reset(new taptenc::TrueCC());

	taptenc::update_t resets;
	for (auto &c : element().resets())
		resets.insert((*c)->semantics().compile());

	return taptenc::Transition {
		element().from()->name(),
		element().to()->name(),
		dynamic_cast<const platform::Component &>(*element().parent()).mangled_name()
			+ "~" + element().from()->name() + "~" + element().to()->name(),
		*clock_formula,
		std::move(resets),
		"" /* TODO: unused? */
	};
}



taptenc::State Semantics<platform::State>::compile()
{
	std::unique_ptr<taptenc::ClockConstraint> clock_formula;
	if (element().clock_formula())
		clock_formula = dynamic_cast<ClockFormulaSemantics &>(element().clock_formula()->semantics()).compile();
	else
		clock_formula.reset(new taptenc::TrueCC());

	return taptenc::State { element().name(), *clock_formula };
}



std::unique_ptr<taptenc::automaton> Semantics<platform::Component>::compile()
{
	platform::Component::Lock l(element().lock());

	std::vector<taptenc::State> states;
	std::vector<taptenc::Transition> transitions;

	for (auto &s : element().states()) {
		taptenc::State ttstate = s->semantics().compile();
		if (element().current_state() == *s)
			ttstate.initial = true;
		states.emplace_back(ttstate);
	}

	for (const unique_ptr<platform::AbstractTransition> &t : element().transitions())
		if (t->is_a<platform::Transition>())
			transitions.emplace_back(t->semantics<platform::Transition>().compile());

	auto rv = std::make_unique<taptenc::automaton>(
		std::move(states),
		std::move(transitions),
		element().name(),
		false /* TODO: set_trap? Wot is det ??*/
	);

	for (const shared_ptr<platform::Clock> &c : element().clocks())
		rv->clocks.emplace(c->semantics().compile());

	return rv;
}


} // namespace gologpp