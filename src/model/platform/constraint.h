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
#include <model/scope.h>
#include <model/reference.h>
#include <model/procedural.h>
#include <model/semantics.h>
#include <execution/clock.h>

#include <model/platform/component.h>
#include <model/platform/reference.h>

#include <tuple>


namespace gologpp {

namespace platform {

class ActionSpec
: public virtual Expression
, public virtual AbstractLanguageElement
, public NoScopeOwner
{
};


class StateSpec
: public virtual Expression
, public virtual AbstractLanguageElement
, public NoScopeOwner
{
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Constraint
: public NoScopeOwner
, public LanguageElement<Constraint, BoolType>
{
public:
	Constraint(ActionSpec *lhs, StateSpec *rhs);

	const ActionSpec &lhs() const;
	const StateSpec &rhs() const;

	virtual string to_string(const string &pfx) const override;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<ActionSpec> lhs_;
	unique_ptr<StateSpec> rhs_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ActionHook
: public ActionSpec
, public LanguageElement<ActionHook, BoolType>
{
public:
	using Hook = DurativeCall::Hook;

	ActionHook(Hook hook, Reference<Action> *action);
	Hook hook() const;
	const Reference<Action> &action() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*action_)

private:
	const Hook hook_;
	const unique_ptr<Reference<Action>> action_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class During
: public ActionSpec
, public LanguageElement<During, BoolType>
{
public:
	During(Reference<Action> *action);
	const Reference<Action> &action() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*action_)

private:
	const unique_ptr<Reference<Action>> action_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class StateAssertion
: public StateSpec
, public LanguageElement<StateAssertion, BoolType>
{
public:
	StateAssertion(Reference<Component> *component, Reference<State> *state);
	const Reference<Component> &component() const;
	const Reference<State> &state() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*component_, *state_)

private:
	const unique_ptr<Reference<Component>> component_;
	const unique_ptr<Reference<State>> state_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
class BooleanConstraintOperation
: public SubjectT
, public LanguageElement<BooleanConstraintOperation<SubjectT>, BoolType>
{
public:
	enum class Operator {
		OR, AND
	};

	BooleanConstraintOperation(SubjectT *lhs, Operator op, SubjectT *rhs);

	const SubjectT &lhs() const;
	const SubjectT &rhs() const;
	Operator op() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<SubjectT> lhs_, rhs_;
	Operator op_;
};

template<class SubjectT>
string to_string(typename BooleanConstraintOperation<SubjectT>::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
class TemporalUnaryOperation
: public SubjectT
, public LanguageElement<TemporalUnaryOperation<SubjectT>, BoolType>
{
public:
	enum class Operator {
		NEXT, PREVIOUS, FUTURE, PAST
	};

	TemporalUnaryOperation(
		SubjectT *subject,
		Operator op,
		boost::optional<fusion_wtf_vector <
			boost::optional<Value>,
			boost::optional<Value>
		> > bound
	);

	const SubjectT &subject() const;
	Operator op() const;
	gologpp::Clock::duration lower_bound() const;
	gologpp::Clock::duration upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_);

private:
	unique_ptr<SubjectT> subject_;
	Operator op_;
	gologpp::Clock::duration lower_bound_, upper_bound_;
};

template<class SubjectT>
string to_string(typename TemporalUnaryOperation<SubjectT>::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class SubjectT>
class TemporalBinaryOperation
: public SubjectT
, public LanguageElement<TemporalBinaryOperation<SubjectT>, BoolType>
{
public:
	enum class Operator {
		UNTIL, SINCE
	};

	TemporalBinaryOperation(
		SubjectT *lhs,
		SubjectT *rhs,
		Operator op,
		Value *lower_bound,
		Value *upper_bound
	);

	const SubjectT &lhs() const;
	const SubjectT &rhs() const;
	Operator op() const;

	gologpp::Clock::duration lower_bound() const;
	gologpp::Clock::duration upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_);

private:
	unique_ptr<SubjectT> lhs_, rhs_;
	Operator op_;
	gologpp::Clock::duration lower_bound_, upper_bound_;
};

template<class SubjectT>
string to_string(typename TemporalBinaryOperation<SubjectT>::Operator op);



} // namespace platform

} // namespace gologpp
