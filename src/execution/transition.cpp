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
: Event<Action>(other.action(), other.argscp())
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
		ground_action_.attach_semantics(implementor);
		for (auto &c : args())
			c->attach_semantics(implementor);
	}
}

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
{ return element(); }



} // namespace gologpp
