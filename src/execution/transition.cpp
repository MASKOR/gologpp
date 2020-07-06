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

#include <model/value.h>
#include <model/action.h>
#include <model/logger.h>

#include "context.h"
#include "transition.h"
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
		semantics_.reset(other.general_semantics<Transition>().copy(*this));
	}
}

Transition::Hook Transition::hook() const
{ return hook_; }

void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		binding().attach_semantics(implementor);
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



GeneralSemantics<Transition>::GeneralSemantics(const Transition &elem, ExecutionContext &context)
: element_(&elem)
, context_(context)
, final_(false)
{}

const Transition &GeneralSemantics<Transition>::element() const
{ return *element_; }

void GeneralSemantics<Transition>::update_element(const Transition *new_element)
{ element_ = new_element; }

ExecutionContext &GeneralSemantics<Transition>::context() const
{ return context_; }


unique_ptr<Plan> GeneralSemantics<Transition>::trans(const ABinding &, History &history)
{
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
					.general_semantics().evaluate(
						element().binding(), history
					)
			)
		)
			context().backend().start_activity(element());
		else
			return nullptr;
	break;
	case Transition::Hook::FINISH:
		if (context().backend().current_state(element()) == Activity::State::FINAL) {
			shared_ptr<Activity> a = context().backend().end_activity(element());

			if (element().target()->senses())
				history.general_semantics<History>().append_sensing_result(a);
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

	history.general_semantics<History>().append(element());

	return unique_ptr<Plan>(new Plan());
}

const ModelElement &GeneralSemantics<Transition>::model_element() const
{ return element(); }

const Instruction &GeneralSemantics<Transition>::instruction() const
{ return element(); }



} // namespace gologpp
