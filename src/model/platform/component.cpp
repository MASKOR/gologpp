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
#include "clock_formula.h"

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

State::State(const string &name, Component &parent, Expression *clock_formula)
: Identifier(name, 0)
, clock_formula_(clock_formula)
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

Transition::Transition(
	Reference<State> *from,
	Reference<State> *to,
	Expression *clock_formula,
	vector<Reference<Clock> *> resets
)
: from_(from)
, to_(to)
, clock_formula_(clock_formula)
, resets_(resets.begin(), resets.end())
{}

const Reference<State> &Transition::from() const
{ return *from_; }

const Reference<State> &Transition::to() const
{ return *to_; }

const vector<unique_ptr<Reference<Clock>>> &Transition::resets() const
{ return resets_; }

const Expression *Transition::clock_formula() const
{ return clock_formula_.get(); }


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


string Transition::to_string(const string &pfx) const
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

Component::Component(const string &name, Scope *own_scope)
: Global(name, {})
, ScopeOwner(own_scope)
, current_state_ { new State("error", *this) }
{
	scope().add_identifier(current_state_);
}

const State &Component::current_state() const
{ return *current_state_; }


vector<shared_ptr<State>> Component::states() const
{ return scope().local_identifiers<State>(); }

const vector<unique_ptr<Transition>> &Component::transitions() const
{ return transitions_; }

const vector<unique_ptr<Transition> > &Component::exog_transitions() const
{ return exog_transitions_; }

vector<shared_ptr<Clock>> Component::clocks() const
{ return scope().local_identifiers<Clock>(); }

void Component::set_exec_context(AExecutionContext &context)
{ exec_context_ = &context; }


void Component::define(
	vector<string> states,
	vector<string> clocks,
	vector<fusion_wtf_vector<string, string>> transitions,
	vector<fusion_wtf_vector<string, string>> exog_transitions
)
{
	for (const string &s : states)
		scope().add_identifier(shared_ptr<State>(new State(s, *this)));
	for (const string &c : clocks)
		scope().add_identifier(shared_ptr<Clock>(new Clock(c, *this)));
	for (auto &pair : transitions)
		transitions_.emplace_back(new Transition(
			new Reference<State>(scope().lookup_identifier<State>(boost::fusion::at_c<0>(pair))),
			new Reference<State>(scope().lookup_identifier<State>(boost::fusion::at_c<1>(pair)))
		));
	for (auto &pair : exog_transitions)
		exog_transitions_.emplace_back(new Transition(
			new Reference<State>(scope().lookup_identifier<State>(boost::fusion::at_c<0>(pair))),
			new Reference<State>(scope().lookup_identifier<State>(boost::fusion::at_c<1>(pair)))
		));
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
	for (unique_ptr<Transition> &t : transitions_)
		t->attach_semantics(f);
	for (unique_ptr<Transition> &et : exog_transitions_)
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


} // namespace platform

} // namespace gologpp
