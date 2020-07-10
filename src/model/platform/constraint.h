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



class ActionSpec
: public virtual AbstractLanguageElement
, public NoScopeOwner
{
protected:
	ActionSpec();
	ActionSpec(const ActionSpec &) = delete;
	ActionSpec(ActionSpec &&) = delete;
	ActionSpec &operator = (const ActionSpec &) = delete;
	ActionSpec &operator = (ActionSpec &&) = delete;

public:
	using ElementType = ActionSpec;

	virtual ~ActionSpec() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	ModelElement *parent();
	const ModelElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);
	virtual bool operator <= (const Type &type) const;

	template<class GologT = ActionSpec>
	GeneralSemantics<GologT> &general_semantics() const
	{ return ModelElement::general_semantics<GologT>(); }

	template<class GologT = ActionSpec>
	Semantics<GologT> &elem_semantics() const
	{ return ModelElement::semantics<GologT>(); }

protected:
	AbstractLanguageElement *parent_;
};



class StateSpec
: public virtual AbstractLanguageElement
, public NoScopeOwner
{
protected:
	StateSpec();
	StateSpec(const StateSpec &) = delete;
	StateSpec(StateSpec &&) = delete;
	StateSpec &operator = (const StateSpec &) = delete;
	StateSpec &operator = (StateSpec &&) = delete;

public:
	using ElementType = StateSpec;

	virtual ~StateSpec() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	ModelElement *parent();
	const ModelElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);
	virtual bool operator <= (const Type &type) const;

	template<class GologT = StateSpec>
	GeneralSemantics<GologT> &general_semantics() const
	{ return ModelElement::general_semantics<GologT>(); }

	template<class GologT = StateSpec>
	Semantics<GologT> &elem_semantics() const
	{ return ModelElement::semantics<GologT>(); }

protected:
	AbstractLanguageElement *parent_;
};



class Constraint
: public NoScopeOwner
, public LanguageElement<Constraint, BoolType>
{
public:
	Constraint(ActionSpec *lhs, StateSpec *rhs);

	const ActionSpec &lhs() const;
	const StateSpec &rhs() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<ActionSpec> lhs_;
	unique_ptr<StateSpec> rhs_;
};



class ActionHook
: public ActionSpec
, public LanguageElement<ActionHook, BoolType>
{
public:
	using Hook = DurativeCall::Hook;

	ActionHook(Hook hook, Reference<Action> *action);
	const Reference<Action> &action() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*action_)

private:
	const Hook hook_;
	const unique_ptr<Reference<Action>> action_;
};



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



template<class OperandT>
class BooleanConstraintOperation
: public OperandT::ElementType
, public virtual AbstractLanguageElement
, public LanguageElement<BooleanConstraintOperation<OperandT>, BoolType>
{
public:
	enum class Operator {
		OR, AND
	};

	BooleanConstraintOperation(OperandT *lhs, Operator op, OperandT *rhs);

	const OperandT &lhs() const;
	const OperandT &rhs() const;
	Operator op() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<OperandT> lhs_, rhs_;
	Operator op_;
};



template<class OperandT>
class TemporalUnaryOperation
: public OperandT::ElementType
, public virtual AbstractLanguageElement
, public LanguageElement<TemporalUnaryOperation<OperandT>, BoolType>
{
public:
	enum class Operator {
		NEXT, PREVIOUS, FUTURE, PAST
	};

	TemporalUnaryOperation(
		OperandT *subject,
		Operator op,
		gologpp::Clock::time_point lower_bound = gologpp::Clock::time_point::min(),
		gologpp::Clock::time_point upper_bound = gologpp::Clock::time_point::max()
	);

	const OperandT &subject() const;
	Operator op() const;
	gologpp::Clock::time_point lower_bound() const;
	gologpp::Clock::time_point upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_);

private:
	unique_ptr<OperandT> subject_;
	Operator op_;
	gologpp::Clock::time_point lower_bound_, upper_bound_;
};



template<class OperandT>
class TemporalBinaryOperation
: public OperandT::ElementType
, public virtual AbstractLanguageElement
, public LanguageElement<TemporalBinaryOperation<OperandT>, BoolType>
{
public:
	enum class Operator {
		UNTIL, SINCE
	};

	TemporalBinaryOperation(
		OperandT *lhs,
		OperandT *rhs,
		Operator op,
		gologpp::Clock::time_point lower_bound = gologpp::Clock::time_point::min(),
		gologpp::Clock::time_point upper_bound = gologpp::Clock::time_point::max()
	);

	const OperandT &lhs() const;
	const OperandT &rhs() const;
	Operator op() const;

	gologpp::Clock::time_point lower_bound() const;
	gologpp::Clock::time_point upper_bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_);

private:
	unique_ptr<OperandT> lhs_, rhs_;
	Operator op_;
	gologpp::Clock::time_point lower_bound_, upper_bound_;
};


} // namespace platform
} // namespace gologpp
