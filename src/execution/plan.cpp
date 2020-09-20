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
#include <model/semantics.h>
#include <model/logger.h>

#include <execution/transition.h>

namespace gologpp {


TimedInstruction::TimedInstruction(unique_ptr<Instruction> &&i)
: instruction_(std::forward<unique_ptr<Instruction>>(i))
, earliest_(Clock::now())
, latest_(Clock::now() + Clock::duration(32767))
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

Clock::time_point TimedInstruction::earliest_timepoint() const
{ return earliest_; }

Clock::time_point TimedInstruction::latest_timepoint() const
{ return latest_; }


void TimedInstruction::set_earliest(Clock::time_point t)
{ earliest_ = t; }

void TimedInstruction::set_latest(Clock::time_point t)
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


Plan &Plan::append(TimedInstruction &&i)
{ 
	elements_.push_back(std::forward<TimedInstruction>(i));
	try {
		// Set timepoints
		Transition &instr = elements_.back().instruction().cast<Transition>();
		if (instr.hook() == Transition::Hook::END) {
			// Find matching START transition
			auto it = std::find_if(elements().rbegin(), elements().rend(), [&] (TimedInstruction &ti) {
				try {
					Transition &trans = ti.instruction().cast<Transition>();
					return trans.hook() == Transition::Hook::START && trans == instr;
				} catch (std::bad_cast &) {
					return false;
				}
			} );

			if (it == elements().rend()) {
				// Not found: use best guess
				elements().back().set_earliest(Clock::now());
				elements().back().set_latest(Clock::now() + instr->duration().max);
			}
			else {
				// Found: add on top
				elements().back().set_earliest(it->earliest_timepoint() + instr->duration().min);
				elements().back().set_latest(it->latest_timepoint() + instr->duration().max);
			}
		}
	} catch (std::bad_cast &) {
	}
	return *this;
}


Plan &Plan::operator =(Plan &&other)
{
	elements_ = std::move(other.elements_);
	return *this;
}


vector<TimedInstruction> &Plan::elements()
{ return elements_; }

const vector<TimedInstruction> &Plan::elements() const
{ return elements_; }

Logger &operator <<(Logger &l, const Plan &p)
{
	l << "{\n";
	for (auto &e : p.elements())
		l << " ["
			<< e.earliest_timepoint() << ", "
			<< e.latest_timepoint()
			<< "] "
			<< e.instruction() << "\n"
		;
	l << "}" << flush;
	return l;
}


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
