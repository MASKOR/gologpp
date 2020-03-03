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

#include "transition.h"
#include "execution.h"
#include "reference.h"
#include "utilities.h"
#include "value.h"


namespace gologpp {


EC_word Semantics<Transition>::plterm()
{
	return ::term(EC_functor(to_string(element().hook()).c_str(), 2),
		reference_term(element()),
		EC_word(ReadylogContext::instance().backend().time().time_since_epoch().count())
	);
}


static const ::std::unordered_map<::std::string, Transition::Hook> name2state {
	{ "start", Transition::Hook::START },
	{ "cancel", Transition::Hook::CANCEL },
	{ "finish", Transition::Hook::FINISH },
	{ "fail", Transition::Hook::FAIL },
	{ "end", Transition::Hook::END }
};


shared_ptr<Transition> gologpp::Semantics<Transition>::transition_from_plterm(EC_word t)
{
	string headname = functor_name(t);

	auto state_it = name2state.find(headname);

	if (state_it == name2state.end())
		return nullptr;

	if (t.arity() != 2)
		throw EngineError("Transition arity must be 2: " + headname);

	t.arg(1, t);
	headname = functor_name(t);

	vector<unique_ptr<Value>> args = plterm_args(t);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(headname);
	shared_ptr<Transition> rv;

	return std::make_shared<Transition>(action, std::move(args), state_it->second);
}



}
