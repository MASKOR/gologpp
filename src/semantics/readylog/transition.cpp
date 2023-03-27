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

#include "transition.h"
#include "execution.h"
#include "reference.h"
#include "utilities.h"

#include <execution/plan.h>
#include <model/logger.h>


namespace gologpp {


EC_word Semantics<Transition>::plterm()
{
	return ::term(EC_functor(to_string(element().hook()).c_str(), 1),
		reference_term(element().ref())
	);
}


static const ::std::unordered_map<::std::string, Transition::Hook> name2state {
	{ "start", Transition::Hook::START },
	{ "cancel", Transition::Hook::CANCEL },
	{ "finish", Transition::Hook::FINISH },
	{ "fail", Transition::Hook::FAIL },
	{ "end", Transition::Hook::END }
};


shared_ptr<Transition> Semantics<Transition>::transition_from_plterm(EC_word t)
{
	string headname = functor_name(t);

	auto state_it = name2state.find(headname);

	if (state_it == name2state.end())
		return nullptr;

	if (t.arity() != 1)
		throw EngineError("Transition arity must be 1: " + headname);

	t.arg(1, t);
	headname = functor_name(t);

	vector<unique_ptr<Value>> args = plterm_args(t);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(Name::demangle(headname));

	shared_ptr<Transition> rv { new Transition { action, std::move(args), state_it->second} };
	rv->attach_semantics(ReadylogContext::instance().semantics_factory());

	return rv;
}


Semantics<Transition> *Semantics<Transition>::copy(const Transition &target_element) const
{ return new Semantics<Transition>(target_element, rl_context()); }


unique_ptr<Plan> Semantics<Transition>::trans(const BindingChain &b, History &history)
{
	BindingChain merged(b);
	merged.emplace_back(&element().ref().binding());

	switch(element().hook())
	{
	case Transition::Hook::CANCEL:
		if (context().backend().current_state(element().ref()) == Activity::State::RUNNING)
			context().backend().cancel_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::START:
		if (
			context().backend().current_state(element().ref()) != Activity::State::RUNNING
			&& static_cast<bool>(
				element().action()->precondition().general_semantics().evaluate(
					merged,
					history
				)
			)
		)
			context().backend().start_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::FINISH:
		if (context().backend().current_state(element().ref()) == Activity::State::FINAL) {
			shared_ptr<Activity> a = context().backend().erase_activity(element());
		}
		else
			return nullptr;
	break;
	case Transition::Hook::FAIL:
		if (context().backend().current_state(element().ref()) == Activity::State::FAILED)
			context().backend().erase_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::END:
		if (
			context().backend().current_state(element().ref()) == Activity::State::FAILED
			|| context().backend().current_state(element().ref()) == Activity::State::FINAL
		)
			context().backend().erase_activity(element());
		else
			return nullptr;
	}

	if (!element().action()->silent()) {
		log(LogLevel::NFY) << "<<< trans: " << element().str() << flush;
		context().set_silent(false);
	}

	history.general_semantics<History>().append(element());

	return unique_ptr<Plan>(new Plan());
}



EC_word Semantics<ExogEvent>::plterm()
{ return element().ref().semantics().plterm(); }

unique_ptr<Plan> Semantics<ExogEvent>::trans(const BindingChain &, History &)
{ throw Bug("trans(...) should not be called on an ExogEvent"); }

Semantics<ExogEvent> *Semantics<ExogEvent>::copy(const ExogEvent &target_element) const
{ return new Semantics<ExogEvent>(target_element, rl_context()); }



}
