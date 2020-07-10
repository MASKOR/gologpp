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

#include <execution/clock.h>

#include "semantics.h"

namespace gologpp {
namespace platform {


class Clock
: public LanguageElement<Clock, VoidType>
, public Identifier
, public NoScopeOwner
, public std::enable_shared_from_this<Clock>
{
public:
	Clock(const string &name);

	DEFINE_ATTACH_SEMANTICS

	virtual string to_string(const string &pfx) const override;
};



class State
: public Identifier
, public NoScopeOwner
, public LanguageElement<State, VoidType>
{
public:
	State(const string &name, const Component &parent, ClockFormula *clock_formula);

	const unique_ptr<ClockFormula> &clock_formula();

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<ClockFormula> clock_formula_;
	const Component &parent;
};



class Transition
: public NoScopeOwner
, public LanguageElement<Transition, VoidType>
{
public:
	Transition(
		const State &from,
		const State &to,
		ClockFormula *clock_formula,
		vector<Reference<Clock> *> resets,
		const Component &parent
	);

	const State &from() const;
	const State &to() const;
	const vector<unique_ptr<Reference<Clock>>> &resets() const;
	const unique_ptr<ClockFormula> &clock_formula() const;

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	const State &from_;
	const State &to_;
	unique_ptr<ClockFormula> clock_formula_;
	vector<unique_ptr<Reference<Clock>>> resets_;

	const Component &parent_;
};



class Component
: public Global
, public ScopeOwner
, public LanguageElement<Component, VoidType>
{
public:
	Component(const string &name);

	const State &current_state() const;
	const vector<State> &states() const;
	const vector<Transition> &transitions() const;
	const vector<Clock> &clocks() const;

	void set_exec_context(AExecutionContext &);

	void define(
		vector<State *> states,
		vector<Transition *> transitions,
		vector<Transition *> exog_transitions,
		vector<Clock *> clocks
	);

	virtual void attach_semantics(gologpp::SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	const State &current_state_;

	vector<unique_ptr<State>> states_;
	vector<unique_ptr<Transition>> transitions_;
	vector<unique_ptr<Transition>> exog_transitions_;
	vector<shared_ptr<Clock>> clocks_;

	AExecutionContext *exec_context_;
};



} // namespace platform
} // namespace gologpp
