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
#include "value.h"
#include "action.h"
#include "execution.h"
#include "logger.h"
#include "history.h"

#include <iostream>


namespace gologpp {


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, Hook hook)
: Grounding<Action>(action, std::move(args))
, hook_(hook)
{}

Transition::Transition(const Transition &other)
: Grounding<Action>(other)
, hook_(other.hook())
{
	if (other.semantics_) {
		semantics_.reset(other.abstract_semantics<Transition>().copy(*this));
	}
}

Transition::Hook Transition::hook() const
{ return hook_; }

void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		params_to_args().attach_semantics(implementor);
		for (unique_ptr<Value> &c : args())
			c->attach_semantics(implementor);
	}
}

string Transition::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(hook()) + "(" + Grounding<Action>::to_string(pfx) + ")"; }

string to_string(Transition::Hook h)
{
	switch (h) {
	case Transition::Hook::START:
		return "start";
	case Transition::Hook::CANCEL:
		return "cancel";
	case Transition::Hook::FINISH:
		return "finish";
	case Transition::Hook::FAIL:
		return "fail";
	case Transition::Hook::END:
		return "end";
	}
	throw Bug(string("Unhandled ") + typeid(h).name());
}



AbstractSemantics<Transition>::AbstractSemantics(const Transition &elem, ExecutionContext &context)
: element_(&elem)
, context_(context)
, final_(false)
{}

const Transition &AbstractSemantics<Transition>::element() const
{ return *element_; }

void AbstractSemantics<Transition>::update_element(const Transition *new_element)
{ element_ = new_element; }

ExecutionContext &AbstractSemantics<Transition>::context() const
{ return context_; }


unique_ptr<Plan> AbstractSemantics<Transition>::trans(const Binding &, History &history)
{
	shared_ptr<Activity> a;

	switch(element().hook())
	{
	case Transition::Hook::CANCEL:
		if (context().backend().current_state(element()) == Activity::State::RUNNING)
			context().backend().cancel_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::START:
		if (
			context().backend().current_state(element()) != Activity::State::RUNNING
			&& static_cast<bool>(
				element().target()->precondition()
					.abstract_semantics().evaluate(
						element().params_to_args(), history
					)
			)
		)
			context().backend().start_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::FINISH:
		if (context().backend().current_state(element()) == Activity::State::FINAL) {
			a = context().backend().end_activity(element());

			if (element().target()->senses())
				history.abstract_semantics<History>().append_sensing_result(a);
		}
		else
			return nullptr;
	break;
	case Transition::Hook::FAIL:
		if (context().backend().current_state(element()) == Activity::State::FAILED)
			context().backend().end_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::END:
		if (
			context().backend().current_state(element()) == Activity::State::FAILED
			|| context().backend().current_state(element()) == Activity::State::FINAL
		)
			context().backend().end_activity(element());
		else
			return nullptr;
	}

	if (!element()->silent()) {
		std::cout << "<<< trans: " << element().str() << std::endl;
		context().set_silent(false);
	}

	history.abstract_semantics<History>().append(element());

	return unique_ptr<Plan>(new Plan());
}

const ModelElement &AbstractSemantics<Transition>::model_element() const
{ return element(); }

const Instruction &AbstractSemantics<Transition>::instruction() const
{ return element(); }



} // namespace gologpp
