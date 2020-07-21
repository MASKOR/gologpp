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

#include "component.h"
#include <model/platform/clock_formula.h>
#include <model/platform/reference.h>

#include <execution/context.h>

namespace gologpp {

namespace platform {


Clock::Clock(const string &name, Component &parent)
: Identifier(name, 0)
{ set_parent(&parent); }

string Clock::to_string(const string &pfx) const
{ return pfx + name(); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

State::State(const string &name, Component &parent, boost::optional<Expression *> clock_formula)
: Identifier(name, 0)
, clock_formula_(clock_formula.get_value_or(nullptr))
{
	set_parent(&parent);
	if (clock_formula_)
		clock_formula_->set_parent(this);
}

const Expression *State::clock_formula() const
{ return clock_formula_.get(); }

void State::attach_semantics(::gologpp::SemanticsFactory &f)
{
	if (semantics_)
		return;
	set_semantics(f.make_semantics(*this));
	if (clock_formula_)
		clock_formula_->attach_semantics(f);
}

string State::to_string(const string &pfx) const
{
	string rv = pfx + name();
	if (clock_formula_)
		rv += " (" + clock_formula()->str() + ")";
	return rv;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

AbstractTransition::AbstractTransition(
	Reference<State> *from,
	Reference<State> *to,
	boost::optional<Expression *> clock_formula,
	boost::optional<vector<Reference<Clock> *>> resets
)
: from_(from)
, to_(to)
, clock_formula_(clock_formula.get_value_or(nullptr))
{
	if (resets)
		resets_ = vector<unique_ptr<Reference<Clock>>>{ resets->begin(), resets->end() };
}

const Reference<State> &AbstractTransition::from() const
{ return *from_; }

const Reference<State> &AbstractTransition::to() const
{ return *to_; }

const vector<unique_ptr<Reference<Clock>>> &AbstractTransition::resets() const
{ return resets_; }

const Expression *AbstractTransition::clock_formula() const
{ return clock_formula_.get(); }


string AbstractTransition::to_string(const string &pfx) const
{
	string rv = pfx + from()->str() + " ->";
	if (clock_formula_)
		rv += "(" + clock_formula_->str() + ")";
	rv += " " + to()->str();
	if (resets_.size()) {
		rv += " resets(";
		for (auto &cl : resets_)
			rv += cl->str() + ", ";
		rv = rv.substr(0, rv.length() - 2);
	}
	return rv;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void Transition::attach_semantics(::gologpp::SemanticsFactory &f)
{
	if (semantics_)
		return;
	set_semantics(f.make_semantics(*this));
	from_->attach_semantics(f);
	to_->attach_semantics(f);
	if (clock_formula_)
		clock_formula_->attach_semantics(f);
	for (unique_ptr<Reference<Clock>> &c : resets_)
		c->attach_semantics(f);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void ExogTransition::attach_semantics(::gologpp::SemanticsFactory &f)
{
	if (semantics_)
		return;
	set_semantics(f.make_semantics(*this));
	from_->attach_semantics(f);
	to_->attach_semantics(f);
	if (clock_formula_)
		clock_formula_->attach_semantics(f);
	for (unique_ptr<Reference<Clock>> &c : resets_)
		c->attach_semantics(f);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

Component::Component(
	Scope *own_scope,
	const string &name
)
: Global(name, {})
, ScopeOwner(own_scope)
, current_state_ { new State("error", *this, boost::none) }
{
	scope().register_identifier(current_state_.get());
}

const State &Component::current_state() const
{ return *current_state_; }


vector<shared_ptr<State>> Component::states() const
{ return scope().local_identifiers<State>(); }

const vector<unique_ptr<Transition>> &Component::transitions() const
{ return transitions_; }

const vector<unique_ptr<ExogTransition> > &Component::exog_transitions() const
{ return exog_transitions_; }

vector<shared_ptr<Clock>> Component::clocks() const
{ return scope().local_identifiers<Clock>(); }

void Component::set_exec_context(AExecutionContext &context)
{ exec_context_ = &context; }

void Component::add_state(State *s)
{ scope().register_identifier(s); }

void Component::add_clock(Clock *c)
{ scope().register_identifier(c); }

void Component::add_transition(Transition *t)
{ transitions_.emplace_back(t); }

void Component::add_exog_transition(ExogTransition *et)
{ exog_transitions_.emplace_back(et); }


void Component::attach_semantics(::gologpp::SemanticsFactory &f)
{
	if (semantics_)
		return;
	set_semantics(f.make_semantics(*this));

	for (shared_ptr<State> &s : states())
		s->attach_semantics(f);
	for (shared_ptr<Clock> &c : clocks())
		c->attach_semantics(f);
	for (unique_ptr<Transition> &t : transitions_)
		t->attach_semantics(f);
	for (unique_ptr<ExogTransition> &et : exog_transitions_)
		et->attach_semantics(f);
}


string Component::to_string(const string &pfx) const
{
	string rv = pfx + "component " + name() + " {" linesep;
	rv += pfx + "states: ";
	for (auto &s : states())
		rv += s->str() + ", ";
	rv = rv.substr(0, rv.length() - 2) + linesep;
	if (clocks().size()) {
		rv += pfx + "clocks: ";
		for (auto &c : clocks())
			rv = c->str() + ", ";
		rv = rv.substr(0, rv.length() - 2) + linesep;
	}
	rv += pfx + "transitions: " linesep;
	for (auto &t : transitions())
		rv += t->to_string(pfx + indent) + linesep;
	rv += pfx + "exog_transitions: " linesep;
	for (auto &et : exog_transitions())
		rv += et->to_string(pfx + indent) + linesep;
	rv += pfx + "}";

	return rv;
}

void Component::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

ModelElement *Component::ref(const vector<Expression *> &/*args*/)
{ return new Reference<Component>(std::dynamic_pointer_cast<Component>(shared_from_this())); }


} // namespace platform

} // namespace gologpp
