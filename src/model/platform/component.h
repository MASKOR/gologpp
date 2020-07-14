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
#include <model/utilities.h>
#include <model/global.h>
#include <model/scope.h>
#include <model/reference.h>

#include <execution/clock.h>

#include "semantics.h"

namespace gologpp {
namespace platform {

class Clock
: public LanguageElement<Clock, VoidType>
, public Identifier
, public ChildElement
, public NoScopeOwner
, public std::enable_shared_from_this<Clock>
{
public:
	using ElementType = ModelElement;

	Clock(const string &name, Component &parent);

	DEFINE_ATTACH_SEMANTICS

	virtual string to_string(const string &pfx) const override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class State
: public Identifier
, public ChildElement
, public NoScopeOwner
, public LanguageElement<State, VoidType>
{
public:
	using ElementType = ModelElement;

	State(const string &name, Component &parent, Expression *clock_formula = nullptr);

	const Expression *clock_formula() const;

	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Expression> clock_formula_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Transition
: public NoScopeOwner
, public LanguageElement<Transition, VoidType>
, public ChildElement
{
private:
	Transition(
		Reference<State> *from,
		Reference<State> *to,
		Expression *clock_formula = nullptr,
		vector<Reference<Clock> *> resets = {}
	);

	friend Component;

public:
	const Reference<State> &from() const;
	const Reference<State> &to() const;
	const vector<unique_ptr<Reference<Clock>>> &resets() const;
	const Expression *clock_formula() const;

	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Reference<State>> from_;
	unique_ptr<Reference<State>> to_;
	unique_ptr<Expression> clock_formula_;
	vector<unique_ptr<Reference<Clock>>> resets_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Component
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
, public LanguageElement<Component, VoidType>
{
public:
	using ElementType = ModelElement;

	Component(const string &name, Scope *own_scope);

	const State &current_state() const;
	vector<shared_ptr<State>> states() const;
	const vector<unique_ptr<Transition>> &transitions() const;
	const vector<unique_ptr<Transition>> &exog_transitions() const;
	vector<shared_ptr<Clock>> clocks() const;

	void set_exec_context(AExecutionContext &);

	void define(
		vector<string> states,
		vector<string> clocks,
		vector<fusion_wtf_vector<string, string>> transitions,
		vector<fusion_wtf_vector<string, string>> exog_transitions
	);

	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	shared_ptr<State> current_state_;

	vector<unique_ptr<Transition>> transitions_;
	vector<unique_ptr<Transition>> exog_transitions_;

	AExecutionContext *exec_context_;
};


} // namespace platform


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<platform::Component>
: public ZeroArityReference<platform::Component>
, public ChildElement
{
public:
	using ZeroArityReference<platform::Component>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<platform::State>
: public ZeroArityReference<platform::State>
, public ChildElement
{
public:
	using ZeroArityReference<platform::State>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<platform::Clock>
: public ZeroArityReference<platform::Clock>
, public ChildElement
{
public:
	using ZeroArityReference<platform::Clock>::ZeroArityReference;

	DEFINE_ATTACH_SEMANTICS
};


} // namespace gologpp
