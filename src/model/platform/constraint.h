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
#include <execution/clock.h>

#include "component.h"

#include <tuple>


namespace gologpp {



template<>
class Reference<platform::Component>
: public ZeroArityReference<platform::Component>
{
public:
	using ZeroArityReference<platform::Component>::ZeroArityReference;

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
};



template<>
class Reference<platform::State>
: public ZeroArityReference<platform::State>
{
public:
	using ZeroArityReference<platform::State>::ZeroArityReference;

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
};



namespace platform {

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Constraint
: public NoScopeOwner
, public LanguageElement<Constraint, BoolType>
{
public:
	Constraint(Expression *lhs, Expression *rhs);

	const Expression &lhs() const;
	const Expression &rhs() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<Expression> lhs_;
	unique_ptr<Expression> rhs_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ActionHook
: public Expression
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
: public Expression
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
: public Expression
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

class BooleanConstraintOperation
: public Expression
, public LanguageElement<BooleanConstraintOperation, BoolType>
{
public:
	enum class Operator {
		OR, AND
	};

	BooleanConstraintOperation(Expression *lhs, Operator op, Expression *rhs);

	const Expression &lhs() const;
	const Expression &rhs() const;
	Operator op() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<Expression> lhs_, rhs_;
	Operator op_;
};

string to_string(typename BooleanConstraintOperation::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class TemporalUnaryOperation
: public Expression
, public LanguageElement<TemporalUnaryOperation, BoolType>
{
public:
	enum class Operator {
		NEXT, PREVIOUS, FUTURE, PAST
	};

	TemporalUnaryOperation(
		Expression *subject,
		Operator op,
		gologpp::Clock::time_point lower_bound = gologpp::Clock::time_point::min(),
		gologpp::Clock::time_point upper_bound = gologpp::Clock::time_point::max()
	);

	const Expression &subject() const;
	Operator op() const;
	gologpp::Clock::time_point lower_bound() const;
	gologpp::Clock::time_point upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_);

private:
	unique_ptr<Expression> subject_;
	Operator op_;
	gologpp::Clock::time_point lower_bound_, upper_bound_;
};

string to_string(typename TemporalUnaryOperation::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class TemporalBinaryOperation
: public Expression
, public virtual AbstractLanguageElement
, public LanguageElement<TemporalBinaryOperation, BoolType>
{
public:
	enum class Operator {
		UNTIL, SINCE
	};

	TemporalBinaryOperation(
		Expression *lhs,
		Expression *rhs,
		Operator op,
		gologpp::Clock::time_point lower_bound = gologpp::Clock::time_point::min(),
		gologpp::Clock::time_point upper_bound = gologpp::Clock::time_point::max()
	);

	const Expression &lhs() const;
	const Expression &rhs() const;
	Operator op() const;

	gologpp::Clock::time_point lower_bound() const;
	gologpp::Clock::time_point upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_);

private:
	unique_ptr<Expression> lhs_, rhs_;
	Operator op_;
	gologpp::Clock::time_point lower_bound_, upper_bound_;
};

string to_string(typename TemporalBinaryOperation::Operator op);



} // namespace platform

} // namespace gologpp
