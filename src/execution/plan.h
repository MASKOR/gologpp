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

#include <model/gologpp.h>
#include <model/expressions.h>

#include "clock.h"



namespace gologpp {


class TimedInstruction
{
public:
	/**
	 * Constructor. Takes ownership of the given @ref Instruction and initializes the
	 * earliest and latest timepoints with the smallest and largest possible values, respectively
	 * (thus representing a temporally unconstrained TimedInstruction)
	 */
	TimedInstruction(unique_ptr<Instruction> &&);
	TimedInstruction(Instruction *);
	TimedInstruction(TimedInstruction &&);

	const Instruction &instruction() const;
	Instruction &instruction();
	Clock::time_point earliest_timepoint() const;
	Clock::time_point latest_timepoint() const;

	void set_earliest(Clock::time_point);
	void set_latest(Clock::time_point);

	bool timepoints_default() const;

	TimedInstruction &operator = (TimedInstruction &&);

private:
	unique_ptr<Instruction> instruction_;
	Clock::time_point earliest_;
	Clock::time_point latest_;
	bool timepoints_default_;
};


class Plan
{
public:
	using container = vector<TimedInstruction>;
	using iterator = container::iterator;
	using reverse_iterator = container::reverse_iterator;

	Plan() = default;
	Plan(Plan &&sub);

	Value max_reward(const Binding &binding, const Reference<Function> &reward_func);

	Plan &append(TimedInstruction &&);
	Plan &append(Plan &&);

	void make_start_slack(Clock::duration slack);

	Plan &operator = (Plan &&);

	vector<TimedInstruction> &elements();
	const vector<TimedInstruction> &elements() const;

	/// @return The next time before which at least one of the next action(s) has to have been executed
	Clock::time_point next_timeout() const;

private:
	/// Reverse search through the plan starting from the given iterator, to match start/end time windows
	/// if @ref back_from_it refers to an end action.
	void sanitize_time_window(iterator back_from_it);

	container elements_;
};


Logger &operator << (Logger &l, const Plan &p);

}
