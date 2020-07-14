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

#include "clock_formula.h"


namespace gologpp {
namespace platform {

ClockBound::ClockBound(Reference<Clock> *clock, ClockBound::Operator op, Value *bound)
: clock_(clock)
, op_(op)
, bound_(bound)
{
	clock_->set_parent(this);
	bound_->set_parent(this);
}

ClockBound::Operator ClockBound::op() const
{ return op_; }

const Reference<Clock> &ClockBound::clock() const
{ return *clock_; }

const Value &ClockBound::bound() const
{ return *bound_; }

string ClockBound::to_string(const string &pfx) const
{ return pfx + clock().str() + gologpp::platform::to_string(op()) + bound().str(); }


string to_string(ClockBound::Operator op)
{
	switch (op) {
	case ClockBound::Operator::GE:
		return " >= ";
	case ClockBound::Operator::GT:
		return " > ";
	case ClockBound::Operator::LE:
		return " <= ";
	case ClockBound::Operator::LT:
		return " < ";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

BooleanClockOperation::BooleanClockOperation(Expression *lhs, BooleanClockOperation::Operator op, Expression *rhs)
: lhs_(lhs)
, rhs_(rhs)
, op_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

BooleanClockOperation::Operator BooleanClockOperation::op() const
{ return op_; }

const Expression &BooleanClockOperation::lhs() const
{ return *lhs_; }

const Expression &BooleanClockOperation::rhs() const
{ return *rhs_; }

string BooleanClockOperation::to_string(const string &pfx) const
{ return pfx + lhs().str() + gologpp::platform::to_string(op()) + rhs().str(); }


string to_string(BooleanClockOperation::Operator op)
{
	switch (op) {
	case BooleanClockOperation::Operator::OR:
		return " | ";
	case BooleanClockOperation::Operator::AND:
		return " & ";
	}
}

}
}
