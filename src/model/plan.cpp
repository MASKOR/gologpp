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
#include "semantics.h"

namespace gologpp {


TimedInstruction::TimedInstruction(unique_ptr<Instruction> &&i)
: instruction_(std::forward<unique_ptr<Instruction>>(i))
, earliest_(TimePoint::min())
, latest_(TimePoint::max())
{}

TimedInstruction::TimedInstruction(Instruction *i)
: TimedInstruction(unique_ptr<Instruction>(i))
{}

TimedInstruction::TimedInstruction(TimedInstruction &&i)
: instruction_(std::move(i.instruction_))
, earliest_(std::move(i.earliest_))
, latest_(std::move(i.latest_))
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


TimedInstruction &TimedInstruction::operator =(TimedInstruction &&i)
{
	instruction_ = std::move(i.instruction_);
	earliest_ = std::move(i.earliest_);
	latest_ = std::move(i.latest_);
	return *this;
}



Plan::Plan(Plan &&sub)
: elements_(std::move(sub.elements_))
{}

void Plan::append(TimedInstruction &&i)
{ elements_.push_back(std::forward<TimedInstruction>(i)); }

Plan &Plan::operator =(Plan &&other)
{
	elements_ = std::move(other.elements_);
	return *this;
}

vector<TimedInstruction> &Plan::elements()
{ return elements_; }

const vector<TimedInstruction> &Plan::elements() const
{ return elements_; }


/*void Plan::attach_semantics(SemanticsFactory &f)
{
	if (semantics_)
		set_semantics(f.make_semantics(*this));
}

string Plan::to_string(const string &pfx) const
{
	string rv = pfx + "Plan {\n";

	for (auto &e : elements())
		rv += pfx + e.instruction().to_string(pfx + indent)
			+ " [" + std::to_string(e.earliest_timepoint().time_since_epoch().count())
			+ ", " + std::to_string(e.latest_timepoint().time_since_epoch().count())
			+ "]\n";

	return rv + pfx + "}\n";
}*/



}
