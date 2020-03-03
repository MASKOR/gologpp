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

#include "plan.h"

namespace gologpp {


TimedInstruction::TimedInstruction(unique_ptr<Instruction> &&i)
: instruction_(std::forward<unique_ptr<Instruction>>(i))
, earliest_(TimePoint::min())
, latest_(TimePoint::max())
{}

TimedInstruction::TimedInstruction(Instruction *i)
: TimedInstruction(unique_ptr<Instruction>(i))
{}

const Instruction &TimedInstruction::instruction() const
{ return *instruction_; }

Instruction &TimedInstruction::instruction()
{ return *instruction_; }

TimePoint TimedInstruction::earliest_timepoint() const
{ return earliest_; }

TimePoint TimedInstruction::latest_timepoint() const
{ return latest_; }

void TimedInstruction::set_earliest(TimePoint t)
{ earliest_ = t; }

void TimedInstruction::set_latest(TimePoint t)
{ latest_ = t; }



void Plan::append_element(TimedInstruction &&i)
{ elements_.push_back(std::forward<TimedInstruction>(i)); }

vector<TimedInstruction> &Plan::elements()
{ return elements_; }

const vector<TimedInstruction> &Plan::elements() const
{ return elements_; }



}
