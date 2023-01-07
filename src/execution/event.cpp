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

namespace gologpp {

template<class ActionT>
Event<ActionT>::Event(shared_ptr<ActionT> action, vector<unique_ptr<Value>> &&args)
: ground_action_(action, std::forward<vector<unique_ptr<Value>>>(args))
{}

template<class ActionT>
Event<ActionT>::Event(shared_ptr<ActionT> action, std::initializer_list<unique_ptr<Value>> &&args)
: ground_action_(action, std::vector<unique_ptr<Value>> { std::forward<vector<unique_ptr<Value>>>(args) })
{}

template<class ActionT>
const vector<Value *> &Event<ActionT>::args()
{ return ground_action_.args(); }


template<class ActionT>
vector<unique_ptr<Value>> Event<ActionT>::argscp() const
{
	vector<unique_ptr<Value>> rv;
	for (Value *v : ground_action_.args())
		rv.push_back(unique_ptr<Value>(v));
	return rv;
}


template
class Event<Action>;

template
class Event<ExogAction>;

template
class Event<platform::SwitchStateAction>;


} // gologpp
