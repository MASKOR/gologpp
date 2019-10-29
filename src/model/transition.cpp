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

namespace gologpp {


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, Hook hook)
: Grounding<Action>(action, std::move(args))
, hook_(hook)
{}

Transition::Hook Transition::hook() const
{ return hook_; }

void Transition::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
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



} // namespace gologpp
