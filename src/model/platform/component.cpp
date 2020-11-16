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
#include <model/logger.h>
#include <model/platform/clock_formula.h>
#include <model/platform/reference.h>
#include <model/platform/switch_state_action.h>
#include <model/platform/component_backend.h>

#include <execution/controller.h>

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

State::State(const string &name, Component &parent, boost::optional<ClockFormula *> clock_formula)
: Identifier(name, 0)
, clock_formula_(clock_formula.get_value_or(nullptr))
{
	set_parent(&parent);
	if (clock_formula_)
		clock_formula_->set_parent(this);
}

const ClockFormula *State::clock_formula() const
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
	boost::optional<ClockFormula *> clock_formula,
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

const ClockFormula *AbstractTransition::clock_formula() const
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
, backend_(nullptr)
, mutex_(new Lock::mutex_type())
{
	scope().register_identifier(new State("error", *this, boost::none));
	error_state_ = scope().lookup_identifier<State>("error");
	current_state_ = error_state_;
	expected_state_ = error_state_;
}


void Component::set_current_state(const shared_ptr<State> &state)
{
	Lock l(lock());
	expected_state_ = state;
	current_state_ = state;
}


void Component::set_current_state_exog(const shared_ptr<State> &state)
{
	Lock l(lock());
	current_state_ = state;
}


shared_ptr<const State> Component::current_state() const
{ return current_state_; }

shared_ptr<State> Component::current_state()
{ return current_state_; }

vector<shared_ptr<State>> Component::states() const
{ return scope().local_identifiers<State>(); }

shared_ptr<State> Component::state(const string name)
{ return scope().lookup_local_identifier<State>(name); }

bool Component::state_changed_exog()
{ return expected_state_ != current_state_; }

const vector<unique_ptr<AbstractTransition>> &Component::transitions() const
{ return transitions_; }

vector<shared_ptr<Clock>> Component::clocks() const
{ return scope().local_identifiers<Clock>(); }


void Component::initialize(AExecutionController &context)
{
	backend_ = context.backend().get_component_backend(this->name());
	backend_->set_model(*this);
	backend_->set_context(context);
	backend_->init();
}


void Component::add_state(State *s)
{
	scope().register_identifier(s);
	s->set_parent(this);

	if (states().size() == 2)
		set_current_state(scope().lookup_local_identifier<State>(s->name()));
}


void Component::add_clock(Clock *c)
{
	scope().register_identifier(c);
	c->set_parent(this);
}


void Component::add_transition(AbstractTransition *t)
{
	transitions_.emplace_back(t);
	t->set_parent(this);
}


void Component::attach_semantics(::gologpp::SemanticsFactory &f)
{
	if (semantics_)
		return;
	set_semantics(f.make_semantics(*this));

	for (shared_ptr<State> &s : states())
		s->attach_semantics(f);
	for (shared_ptr<Clock> &c : clocks())
		c->attach_semantics(f);
	for (unique_ptr<AbstractTransition> &t : transitions_)
		t->attach_semantics(f);
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
	rv += pfx + "}";

	return rv;
}

void Component::compile(AExecutionController &ctx)
{ initialize(ctx); }

ModelElement *Component::ref(const vector<Expression *> &/*args*/)
{ return new Reference<Component>(std::dynamic_pointer_cast<Component>(shared_from_this())); }


void Component::switch_state(const string &state_name)
{
	Component::Lock l(lock());

	shared_ptr<State> tgt = scope().lookup_identifier<State>(state_name);
	if (!tgt)
		throw Bug(string(__func__) + ": Invalid target state: " + state_name);

	if (!find_transition<Transition>(*current_state(), *tgt))
		throw ComponentError(
			"No transition from " + current_state()->name()
			+ " to " + state_name + " in component " + name()
		);

	backend_->switch_state(state_name);
	set_current_state(tgt);
}

ComponentBackend &Component::backend()
{ return *backend_; }

Component::Lock Component::lock() const
{ return Lock(*mutex_); }


template<class TransitionT>
const TransitionT *Component::find_transition(const State &from, const State &to) const
{
	auto it = std::find_if(
		transitions().begin(),
		transitions().end(),
		[&] (const unique_ptr<AbstractTransition> &t) {
			try {
				TransitionT &tt = dynamic_cast<TransitionT &>(*t);
				return *tt.from().target() == from && *tt.to().target() == to;
			} catch (std::bad_cast &) {
				return false;
			}
		}
	);
	if (it == transitions().end())
		return nullptr;
	else
		return dynamic_cast<const TransitionT *>(it->get());
}

template
const Transition *Component::find_transition(const State &from, const State &to) const;

template
const ExogTransition *Component::find_transition(const State &from, const State &to) const;

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////



} // namespace platform

} // namespace gologpp
