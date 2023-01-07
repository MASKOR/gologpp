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

#pragma once

#include <model/gologpp.h>
#include <model/language.h>
#include <model/expressions.h>
#include <model/value.h>
#include <model/reference.h>

#include <map>

namespace gologpp {


class AbstractEvent
{
public:
	virtual const Reference<AbstractAction, Value> &ref() const = 0;
	virtual Reference<AbstractAction, Value> &ref() = 0;
};


template<class ActionT>
class Event
: public AbstractEvent
{
public:
	Event(shared_ptr<ActionT> action, vector<unique_ptr<Value>> &&args);
	Event(shared_ptr<ActionT> action, std::initializer_list<unique_ptr<Value>> &&args);
	Event(shared_ptr<ActionT> action, std::initializer_list<Value *> &&args);
	shared_ptr<ActionT> &action() const;

	const vector<Value *> &args();
	vector<unique_ptr<Value>> argscp() const;
	virtual const Reference<ActionT, Value> &ref() const override;
	virtual Reference<ActionT, Value> &ref() override;


protected:
	Reference<ActionT, Value> ground_action_;
};


}
