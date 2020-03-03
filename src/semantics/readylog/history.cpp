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

#include <model/transition.h>

#include "history.h"
#include "action.h"
#include "execution.h"
#include "utilities.h"
#include "value.h"
#include "transition.h"
#include "activity.h"

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

shared_ptr<Transition> Semantics<History>::get_last_transition()
{
	if (!has_changed())
		return nullptr;

	EC_word head = get_history_head();

	return Semantics<Transition>::transition_from_plterm(head);
}


void Semantics<History>::append(shared_ptr<Grounding<AbstractAction>> trans)
{ extend_history(::list(trans->semantics().plterm(), current_history())); }

void Semantics<History>::append_sensing_result(shared_ptr<Activity> a)
{ extend_history(::list(a->semantics().sensing_result(), current_history())); }

EC_word Semantics<History>::current_history() const
{ return readylog_history_; }

bool Semantics<History>::has_changed() const
{ return has_changed_; }


void Semantics<History>::extend_history(EC_word h)
{
	ManagedTerm new_history(h);
	has_changed_ = ReadylogContext::instance().ec_query(::term(EC_functor(">", 2),
		::term(EC_functor("length", 1), new_history),
		::term(EC_functor("length", 1), current_history())
	));
	if (has_changed_)
		readylog_history_ = h;
}


bool Semantics<History>::should_progress() const
{
	return ReadylogContext::instance().ec_query(
		::term(EC_functor("should_progress", 1),
			current_history()
		)
	);
}


void Semantics<History>::progress()
{
	EC_ref New_history;
	EC_word query = ::term(EC_functor("update_current_val", 2),
		current_history(),
		New_history
	);
	EC_word q2 = query;
	if (!ReadylogContext::instance().ec_query(query))
		throw EclipseError("Failed to progress: " + ReadylogContext::instance().to_string(q2));

	readylog_history_ = New_history;
}





} // namespace gologpp

