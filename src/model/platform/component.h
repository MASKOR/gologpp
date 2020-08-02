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
#include <model/error.h>

#include <execution/clock.h>

#include <model/platform/semantics.h>

#include <mutex>

namespace gologpp {
namespace platform {


class ComponentError : public EngineError {
public:
	using EngineError::EngineError;
};


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

	State(const string &name, Component &parent, boost::optional<Expression *> clock_formula);

	const Expression *clock_formula() const;

	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Expression> clock_formula_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractTransition
: public NoScopeOwner
, public ChildElement
{
public:
	AbstractTransition(
		Reference<State> *from,
		Reference<State> *to,
		boost::optional<Expression *> clock_formula,
		boost::optional<vector<Reference<Clock> *>> resets
	);

	const Reference<State> &from() const;
	const Reference<State> &to() const;
	const vector<unique_ptr<Reference<Clock>>> &resets() const;
	const Expression *clock_formula() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<Reference<State>> from_;
	unique_ptr<Reference<State>> to_;
	unique_ptr<Expression> clock_formula_;
	vector<unique_ptr<Reference<Clock>>> resets_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Transition
: public AbstractTransition
, public LanguageElement<Transition, VoidType>
{
public:
	using AbstractTransition::AbstractTransition;
	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ExogTransition
: public AbstractTransition
, public LanguageElement<ExogTransition, VoidType>
{
public:
	using AbstractTransition::AbstractTransition;
	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ComponentBackend {
public:
	void set_model(Component &model);
	Component &model();
	void set_context(AExecutionContext &context);
	AExecutionContext &context();

	void exog_state_change(const string &state_name);

	virtual void switch_state(const string &state_name) = 0;
	virtual void init() = 0;
	virtual void terminate() = 0;

private:
	Component *model_;
	AExecutionContext *exec_context_;
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
	using Lock = std::unique_lock<std::mutex>;

	Component(Scope *own_scope, const string &name);

	void set_current_state(const shared_ptr<State> &);
	const State &current_state() const;
	vector<shared_ptr<State>> states() const;
	const vector<unique_ptr<AbstractTransition>> &transitions() const;
	vector<shared_ptr<Clock>> clocks() const;

	void initialize(AExecutionContext &);

	void add_state(State *);
	void add_clock(Clock *);
	void add_transition(AbstractTransition *);

	template<class GologT>
	Reference<GologT> *get_ref(const string &name);

	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

	virtual void compile(gologpp::AExecutionContext &) override;
	virtual ModelElement *ref(const vector<Expression *> &args = {}) override;

	void switch_state(const string &state_name);

	template<class TransitionT>
	const TransitionT &find_transition(const State &from, const State &to) const;

	ComponentBackend &backend();

	Lock lock() const;

private:
	shared_ptr<State> current_state_;
	shared_ptr<State> error_state_;
	vector<unique_ptr<AbstractTransition>> transitions_;
	ComponentBackend *backend_;

	unique_ptr<std::mutex> mutex_;
};


template<class GologT>
Reference<GologT> *Component::get_ref(const string &name)
{ return new Reference<GologT>(scope().lookup_identifier<GologT>(name)); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class DummyComponentBackend : public ComponentBackend {
public:
	virtual void switch_state(const string &state_name) override;
	virtual void init() override;
	virtual void terminate() override;
};

} // namespace platform

} // namespace gologpp
