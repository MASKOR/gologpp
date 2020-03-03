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

#include <chrono>
#include <tuple>
#include <initializer_list>

#include "gologpp.h"

#include "gologpp.h"
#include "expressions.h"



namespace gologpp {


using TimePoint = std::chrono::steady_clock::time_point;


class TimedInstruction {
public:
	/**
		* Constructor. Takes ownership of the given @ref Instruction and initializes the
		* earliest and latest timepoints with the smallest and largest possible values, respectively
		* (thus representing a temporally unconstrained TimedInstruction)
		*/
	TimedInstruction(unique_ptr<Instruction> &&);
	TimedInstruction(Instruction *);

	const Instruction &instruction() const;
	Instruction &instruction();
	TimePoint earliest_timepoint() const;
	TimePoint latest_timepoint() const;

	void set_earliest(TimePoint);
	void set_latest(TimePoint);

private:
	unique_ptr<Instruction> instruction_;
	TimePoint earliest_;
	TimePoint latest_;
};



class Plan {
public:
	Plan() = default;
	Plan(std::initializer_list<unique_ptr<Instruction>> &&step);
	Plan(const Plan &sub);

	Value max_reward(const Binding &binding, const Reference<Function> &reward_func);

	void append_element(TimedInstruction &&);

	vector<TimedInstruction> &elements();
	const vector<TimedInstruction> &elements() const;

private:
	vector<TimedInstruction> elements_;
};


}
