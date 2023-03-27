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

#include <execution/plan.h>

#include <model/value.h>
#include <model/action.h>
#include <model/logger.h>

#include "controller.h"
#include "transition.h"


namespace gologpp {


Transition::Transition(const Transition &other)
: Event<Action>(other)
, hook_(other.hook())
{
	if (other.semantics_)
		semantics_.reset(other.general_semantics<Transition>().copy(*this));
}

Transition::Transition(shared_ptr<Action> action, vector<unique_ptr<Value> > &&value, Hook hook)
: Event<Action>(action, std::move(value))
, hook_(hook)
{}

Transition::Hook Transition::hook() const
{ return hook_; }

string Transition::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(hook()) + "(" + ground_action_.to_string(pfx) + ")"; }




GeneralSemantics<Transition>::GeneralSemantics(const Transition &elem, AExecutionController &context)
: element_(&elem)
, context_(context)
, final_(false)
{}

const Transition &GeneralSemantics<Transition>::element() const
{ return *element_; }

void GeneralSemantics<Transition>::update_element(const Transition *new_element)
{ element_ = new_element; }

AExecutionController &GeneralSemantics<Transition>::context() const
{ return context_; }

const ModelElement &GeneralSemantics<Transition>::model_element() const
{ return element(); }

const Instruction &GeneralSemantics<Transition>::instruction() const
{ return static_cast<const Instruction &>(element()); }



ExogEvent::ExogEvent(const ExogEvent &other)
: Event<ExogAction>(other)
{
	if (other.semantics_)
		semantics_.reset(other.general_semantics<ExogEvent>().copy(*this));
}



} // namespace gologpp
