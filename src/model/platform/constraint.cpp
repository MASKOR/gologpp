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

#include "constraint.h"
#include <boost/fusion/include/at_c.hpp>

namespace gologpp {
namespace platform {


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////


Constraint::Constraint(Expression *lhs, Expression *rhs)
: lhs_(lhs)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

const Expression &Constraint::lhs() const
{ return *lhs_; }

const Expression &Constraint::rhs() const
{ return *rhs_; }

string Constraint::to_string(const string &pfx) const
{ return pfx + lhs().str() + ": " + rhs().str(); }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ActionHook::ActionHook(ActionHook::Hook hook, Reference<Action> *action)
: hook_(hook)
, action_(action)
{ action_->set_parent(this); }

ActionHook::Hook ActionHook::hook() const
{ return hook_; }

const Reference<Action> &ActionHook::action() const
{ return *action_; }

string ActionHook::to_string(const string &pfx) const
{ return pfx + gologpp::to_string(hook()) + "(" + action().str() + ")"; }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

During::During(Reference<Action> *action)
: action_(action)
{ action_->set_parent(this); }

const Reference<Action> &During::action() const
{ return *action_; }

string During::to_string(const string &pfx) const
{ return pfx + "during(" + action().str() + ")"; }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

StateAssertion::StateAssertion(Reference<Component> *component, Reference<State> *state)
: component_(component)
, state_(state)
{
	component_->set_parent(this);
	state_->set_parent(this);
}

const Reference<Component> &StateAssertion::component() const
{ return *component_; }

const Reference<State> &StateAssertion::state() const
{ return *state_; }

string StateAssertion::to_string(const string &pfx) const
{ return pfx + "state(" + component().str() + ") = " + state().str(); }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

BooleanConstraintOperation::BooleanConstraintOperation(
	Expression *lhs,
	BooleanConstraintOperation::Operator op,
	Expression *rhs
)
: lhs_(lhs)
, rhs_(rhs)
, op_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

const Expression &BooleanConstraintOperation::lhs() const
{ return *lhs_; }

const Expression &BooleanConstraintOperation::rhs() const
{ return *rhs_; }

typename BooleanConstraintOperation::Operator BooleanConstraintOperation::op() const
{ return op_; }

string BooleanConstraintOperation::to_string(const string &pfx) const
{ return pfx + lhs().str() + " " + gologpp::platform::to_string(op()) + " " + rhs().str(); }


string to_string(typename BooleanConstraintOperation::Operator op)
{
	switch (op) {
	case BooleanConstraintOperation::Operator::OR:
		return "|";
	case BooleanConstraintOperation::Operator::AND:
		return "&";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

TemporalUnaryOperation::TemporalUnaryOperation(
	Expression *subject,
	TemporalUnaryOperation::Operator op,
	boost::optional<fusion_wtf_vector <
		boost::optional<gologpp::Clock::time_point>,
		boost::optional<gologpp::Clock::time_point>
	> > bound
)
: subject_(subject)
, op_(op)
{
	subject_->set_parent(this);
	if (!bound) {
		lower_bound_ = gologpp::Clock::time_point::min();
		upper_bound_ = gologpp::Clock::time_point::max();
	}
	else {
		lower_bound_ = boost::fusion::at_c<0>(bound.get())
			.get_value_or(gologpp::Clock::time_point::min());
		upper_bound_ = boost::fusion::at_c<1>(bound.get())
			.get_value_or(gologpp::Clock::time_point::max());
	}
}

const Expression &TemporalUnaryOperation::subject() const
{ return *subject_; }

typename TemporalUnaryOperation::Operator TemporalUnaryOperation::op() const
{ return op_; }

gologpp::Clock::time_point TemporalUnaryOperation::lower_bound() const
{ return lower_bound_; }

gologpp::Clock::time_point TemporalUnaryOperation::upper_bound() const
{ return upper_bound_; }


string TemporalUnaryOperation::to_string(const string &pfx) const
{
	return pfx + gologpp::platform::to_string(op()) + "["
		+ gologpp::to_string(lower_bound()) + ", " + gologpp::to_string(upper_bound())
		+ "] " + subject().str();
}


string to_string(typename TemporalUnaryOperation::Operator op)
{
	switch (op) {
	case TemporalUnaryOperation::Operator::NEXT:
		return "next";
	case TemporalUnaryOperation::Operator::PREVIOUS:
		return "previous";
	case TemporalUnaryOperation::Operator::FUTURE:
		return "future";
	case TemporalUnaryOperation::Operator::PAST:
		return "past";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

TemporalBinaryOperation::TemporalBinaryOperation(
	Expression *lhs,
	Expression *rhs,
	TemporalBinaryOperation::Operator op,
	boost::optional<fusion_wtf_vector <
		boost::optional<gologpp::Clock::time_point>,
		boost::optional<gologpp::Clock::time_point>
	> > bound
)
: lhs_(lhs)
, rhs_(rhs)
, op_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
	if (!bound) {
		lower_bound_ = gologpp::Clock::time_point::min();
		upper_bound_ = gologpp::Clock::time_point::max();
	}
	else {
		lower_bound_ = boost::fusion::at_c<0>(bound.get())
			.get_value_or(gologpp::Clock::time_point::min());
		upper_bound_ = boost::fusion::at_c<1>(bound.get())
			.get_value_or(gologpp::Clock::time_point::max());
	}
}

const Expression &TemporalBinaryOperation::lhs() const
{ return *lhs_; }

const Expression &TemporalBinaryOperation::rhs() const
{ return *rhs_; }

typename TemporalBinaryOperation::Operator TemporalBinaryOperation::op() const
{ return op_; }

gologpp::Clock::time_point TemporalBinaryOperation::lower_bound() const
{ return lower_bound_; }

gologpp::Clock::time_point TemporalBinaryOperation::upper_bound() const
{ return upper_bound_; }


string TemporalBinaryOperation::to_string(const string &pfx) const
{
	return pfx + lhs().str() + gologpp::platform::to_string(op()) + "[" + gologpp::to_string(lower_bound()) + ", "
		+ gologpp::to_string(upper_bound()) + "] " + rhs().str();
}


string to_string(typename TemporalBinaryOperation::Operator op)
{
	switch (op) {
	case TemporalBinaryOperation::Operator::UNTIL:
		return "until";
	case TemporalBinaryOperation::Operator::SINCE:
		return "since";
	}
}



} // namespace platform


} // namespace gologpp
