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

#include "history.h"
#include "utilities.h"
#include "action.h"
#include "execution.h"
#include "value.h"

#include <model/transition.h>


#include <iostream>
#include <unordered_map>

namespace gologpp {


Semantics<History>::Semantics(History &history)
: AbstractSemantics<History>(history)
, has_changed_(false)
{ readylog_history_ = ::list(EC_atom("s0"), ::nil()); }


EC_word Semantics<History>::get_history_head()
{
	EC_word head, tail;
	if (current_history().is_list(head, tail) != EC_succeed)
		throw Bug("ReadyLog history is not a list");
	return head;
}


string Semantics<History>::get_head_name(EC_word head)
{
	EC_functor headfunctor;
	EC_atom head_atom;
	if (head.functor(&headfunctor) == EC_succeed)
		return headfunctor.name();
	else if (head.is_atom(&head_atom) == EC_succeed)
		return head_atom.name();

	throw Bug("Unknown term in history");
}


vector<unique_ptr<Value>> get_args(EC_word head) {
	EC_word term;
	vector<unique_ptr<Value>> rv;

	for (int j = 1; j <= head.arity(); j++) {
		head.arg(j,term);
		Value *v = new Value(pl_term_to_value(term));

		if (!v)
			throw Bug("Invalid argument #" + std::to_string(j) + " in expression " + ReadylogContext::instance().to_string(head));

		rv.emplace_back(v);
	}

	return rv;
}


static const std::unordered_map<string, Transition::Hook> name2state {
	{ "start", Transition::Hook::START },
	{ "stop", Transition::Hook::STOP },
	{ "finish", Transition::Hook::FINISH },
	{ "fail", Transition::Hook::FAIL },
};


shared_ptr<Transition> Semantics<History>::get_last_transition()
{
	if (!has_changed())
		return nullptr;

	EC_word head = get_history_head();
	string headname = get_head_name(head);

	auto state_it = name2state.find(headname);

	if (state_it == name2state.end())
		return nullptr;

	if (head.arity() != 2)
		throw EngineError("Transition arity must be 2: " + headname);

	head.arg(1, head);
	headname = get_head_name(head);

	vector<unique_ptr<Value>> args = get_args(head);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(headname, arity_t(head.arity()));
	shared_ptr<Transition> rv;

	return std::make_shared<Transition>(action, std::move(args), state_it->second);
}


void Semantics<History>::append_exog(shared_ptr<Grounding<AbstractAction>> trans)
{ set_current_history(::list(trans->semantics().plterm(), current_history())); }

void Semantics<History>::append_sensing_result(shared_ptr<Activity> a)
{ set_current_history(::list(a->semantics().sensing_result(), current_history())); }

EC_word Semantics<History>::current_history()
{ return readylog_history_; }

bool Semantics<History>::has_changed() const
{ return has_changed_; }

void Semantics<History>::set_current_history(EC_word h)
{
	ManagedTerm new_history(h);
	has_changed_ = ReadylogContext::instance().ec_query(::term(EC_functor(">", 2),
		::term(EC_functor("length", 1), new_history),
		::term(EC_functor("length", 1), current_history())
	));
	if (has_changed_)
		readylog_history_ = h;
}



} // namespace gologpp

