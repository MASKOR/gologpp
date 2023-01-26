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


#include "event.h"
#include <model/action.h>
#include <model/variable.h>
#include <model/platform/switch_state_action.h>
#include <model/semantics.h>

namespace gologpp {

string AbstractEvent::to_string(const string &pfx) const
{ return ref().to_string(pfx); }

template<class ActionT>
Event<ActionT>::Event(shared_ptr<ActionT> action, vector<unique_ptr<Value>> &&args)
: ground_action_(action, std::move(args))
{}

template<class ActionT>
Event<ActionT>::Event(shared_ptr<ActionT> action, std::initializer_list<unique_ptr<Value>> &&args)
: ground_action_(action, std::vector<unique_ptr<Value>> { std::move(args) })
{}

template<class ActionT>
Event<ActionT>::Event(shared_ptr<ActionT> action, std::initializer_list<Value *> &&args)
: ground_action_(action, std::vector<unique_ptr<Value>> { args.begin(), args.end() })
{}

template<class ActionT>
Event<ActionT>::Event(Event<ActionT> &&other)
: ground_action_(std::move(other.ground_action_))
{}

template<class ActionT>
Event<ActionT>::Event(const Event<ActionT> &other)
: ground_action_(other.ground_action_)
{
	if (other.semantics_)
		semantics_.reset(other.template general_semantics<Event<ActionT>>().copy(*this));
}

template<class ActionT>
shared_ptr<ActionT> Event<ActionT>::action() const
{ return ground_action_.target(); }

template<class ActionT>
const vector<Value *> &Event<ActionT>::args()
{ return ground_action_.args(); }

template<class ActionT>
const Reference<ActionT, Value> &Event<ActionT>::ref() const
{ return ground_action_; }

template<class ActionT>
Reference<ActionT, Value> &Event<ActionT>::ref()
{ return ground_action_; }


template
class Event<Action>;

template
class Event<ExogAction>;

template
class Event<platform::SwitchStateAction>;


} // gologpp
