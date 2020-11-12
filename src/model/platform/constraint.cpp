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


Constraint::Constraint(ActionSpec *lhs, StateSpec *rhs)
: lhs_(lhs)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

const ActionSpec &Constraint::lhs() const
{ return *lhs_; }

const StateSpec &Constraint::rhs() const
{ return *rhs_; }

string Constraint::to_string(const string &pfx) const
{ return pfx + lhs().str() + ": " + rhs().str(); }

Scope &Constraint::parent_scope()
{ return global_scope(); }

const Scope &Constraint::parent_scope() const
{ return global_scope(); }


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
{ return pfx + "state(" + component().name() + ") = " + state().str(); }


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
BooleanConstraintOperation<SubjectT>::BooleanConstraintOperation(
	SubjectT *lhs,
	BooleanConstraintOperation::Operator op,
	SubjectT *rhs
)
: lhs_(lhs)
, rhs_(rhs)
, op_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

template<class SubjectT>
const SubjectT &BooleanConstraintOperation<SubjectT>::lhs() const
{ return *lhs_; }

template<class SubjectT>
const SubjectT &BooleanConstraintOperation<SubjectT>::rhs() const
{ return *rhs_; }

template<class SubjectT>
typename BooleanConstraintOperation<SubjectT>::Operator BooleanConstraintOperation<SubjectT>::op() const
{ return op_; }

template<class SubjectT>
string BooleanConstraintOperation<SubjectT>::to_string(const string &pfx) const
{ return pfx + lhs().str() + " " + gologpp::platform::to_string<SubjectT>(op()) + " " + rhs().str(); }


template<class SubjectT>
string to_string(typename BooleanConstraintOperation<SubjectT>::Operator op)
{
	switch (op) {
	case BooleanConstraintOperation<SubjectT>::Operator::OR:
		return "|";
	case BooleanConstraintOperation<SubjectT>::Operator::AND:
		return "&";
	}
	throw Bug("Unhandled BooleanConstraintOperation::Operator");
}

template
class BooleanConstraintOperation<ActionSpec>;

template
class BooleanConstraintOperation<StateSpec>;

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
TemporalUnaryOperation<SubjectT>::TemporalUnaryOperation(
	SubjectT *subject,
	TemporalUnaryOperation::Operator op,
	boost::optional<fusion_wtf_vector <
		boost::optional<Value>,
		boost::optional<Value>
	> > bound
)
: subject_(subject)
, op_(op)
{
	using namespace boost::fusion;

	subject_->set_parent(this);

	gologpp::Clock::duration tmin = gologpp::Clock::duration::min();
	gologpp::Clock::duration tmax = gologpp::Clock::duration::max();
	if (bound) {
		if (at_c<0>(bound.get()))
			lower_bound_ = gologpp::Clock::duration {
				at_c<0>(bound.get()).get().numeric_convert<gologpp::Clock::rep>()
			};
		else
			lower_bound_ = tmin;

		if (at_c<1>(bound.get()))
			upper_bound_ = gologpp::Clock::duration {
				at_c<1>(bound.get()).get().numeric_convert<gologpp::Clock::rep>()
			};
		else
			upper_bound_ = tmax;
	}
	else {
		lower_bound_ = tmin;
		upper_bound_ = tmax;
	}
}

template<class SubjectT>
const SubjectT &TemporalUnaryOperation<SubjectT>::subject() const
{ return *subject_; }

template<class SubjectT>
typename TemporalUnaryOperation<SubjectT>::Operator TemporalUnaryOperation<SubjectT>::op() const
{ return op_; }

template<class SubjectT>
gologpp::Clock::duration TemporalUnaryOperation<SubjectT>::lower_bound() const
{ return lower_bound_; }

template<class SubjectT>
gologpp::Clock::duration TemporalUnaryOperation<SubjectT>::upper_bound() const
{ return upper_bound_; }


template<class SubjectT>
string TemporalUnaryOperation<SubjectT>::to_string(const string &pfx) const
{
	string rv = pfx + gologpp::platform::to_string<SubjectT>(op()) + "[";
	if (lower_bound() != gologpp::Clock::duration::min())
		rv += gologpp::to_string(lower_bound());
	else
		rv += "*";
	rv += ", ";
	if (upper_bound() != gologpp::Clock::duration::max())
		rv += gologpp::to_string(upper_bound());
	else
		rv += "*";
	rv += "] " + subject().str();

	return rv;
}


template<class SubjectT>
string to_string(typename TemporalUnaryOperation<SubjectT>::Operator op)
{
	switch (op) {
	case TemporalUnaryOperation<SubjectT>::Operator::NEXT:
		return "next";
	case TemporalUnaryOperation<SubjectT>::Operator::PREVIOUS:
		return "previous";
	case TemporalUnaryOperation<SubjectT>::Operator::FUTURE:
		return "future";
	case TemporalUnaryOperation<SubjectT>::Operator::PAST:
		return "past";
	}
	throw Bug("Unhandled TemporalUnaryOperation::Operator");
}

template
class TemporalUnaryOperation<StateSpec>;

template
class TemporalUnaryOperation<ActionSpec>;



/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
TemporalBinaryOperation<SubjectT>::TemporalBinaryOperation(
	SubjectT *lhs,
	SubjectT *rhs,
	TemporalBinaryOperation<SubjectT>::Operator op,
	Value *lower_bound,
	Value *upper_bound
)
: lhs_(lhs)
, rhs_(rhs)
, op_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);

	lower_bound_ = gologpp::Clock::duration { static_cast<gologpp::Clock::rep>(*lower_bound) };
	upper_bound_ = gologpp::Clock::duration { static_cast<gologpp::Clock::rep>(*upper_bound) };
	delete lower_bound;
	delete upper_bound;
}

template<class SubjectT>
const SubjectT &TemporalBinaryOperation<SubjectT>::lhs() const
{ return *lhs_; }

template<class SubjectT>
const SubjectT &TemporalBinaryOperation<SubjectT>::rhs() const
{ return *rhs_; }

template<class SubjectT>
typename TemporalBinaryOperation<SubjectT>::Operator TemporalBinaryOperation<SubjectT>::op() const
{ return op_; }

template<class SubjectT>
gologpp::Clock::duration TemporalBinaryOperation<SubjectT>::lower_bound() const
{ return lower_bound_; }

template<class SubjectT>
gologpp::Clock::duration TemporalBinaryOperation<SubjectT>::upper_bound() const
{ return upper_bound_; }


template<class SubjectT>
string TemporalBinaryOperation<SubjectT>::to_string(const string &pfx) const
{
	string rv = pfx + lhs().str() + " " + gologpp::platform::to_string<SubjectT>(op()) + "[";
	if (lower_bound() != gologpp::Clock::duration::min())
		rv += gologpp::to_string(lower_bound());
	else
		rv += "*";
	rv += ", ";
	if (upper_bound() != gologpp::Clock::duration::max())
		rv += gologpp::to_string(upper_bound());
	else
		rv += "*";
	rv += "] " + rhs().str();
	return rv;
}


template<class SubjectT>
string to_string(typename TemporalBinaryOperation<SubjectT>::Operator op)
{
	switch (op) {
	case TemporalBinaryOperation<SubjectT>::Operator::UNTIL:
		return "until";
	case TemporalBinaryOperation<SubjectT>::Operator::SINCE:
		return "since";
	}
	throw Bug("Unhandled TemporalBinaryOperation::Operator");
}


template
class TemporalBinaryOperation<StateSpec>;

template
class TemporalBinaryOperation<ActionSpec>;


} // namespace platform


} // namespace gologpp
