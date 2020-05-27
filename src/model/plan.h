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

#include "gologpp.h"

#include <initializer_list>

namespace gologpp {


class Plan {
public:
	Plan(std::initializer_list<unique_ptr<Instruction>> &&step);
	Plan(const Plan &sub);

	Value max_reward(const Binding &binding, const Reference<Function> &reward_func);

private:
	vector<unique_ptr<Instruction>> steps_;
};


}
