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

#include "value.h"
#include "transition.h"
#include "execution.h"
#include "reference.h"
#include "utilities.h"

#include <execution/plan.h>


namespace gologpp {


Semantics<Transition>::Semantics(const Transition &elem, ReadylogContext &context)
: GeneralSemantics<Transition>(elem, context)
{}


EC_word Semantics<Transition>::plterm()
{
	return ::term(EC_functor(to_string(element().hook()).c_str(), 1),
		reference_term(element())
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

	if (t.arity() != 1)
		throw EngineError("Transition arity must be 1: " + headname);

	t.arg(1, t);
	headname = functor_name(t);

	vector<unique_ptr<Expression>> args = plterm_args(t);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(Name::demangle(headname));

	shared_ptr<Transition> rv { new Transition { action, std::move(args), state_it->second} };
	rv->attach_semantics(ReadylogContext::instance().semantics_factory());

	return rv;
}


Semantics<Transition> *Semantics<Transition>::copy(const Transition &target_element) const
{ return new Semantics<Transition>(target_element, rl_context()); }

unique_ptr<Plan> Semantics<Transition>::trans(const BindingChain &b, History &h)
{ return GeneralSemantics<Transition>::trans(b, h); }

const Instruction &Semantics<Transition>::instruction() const
{ return GeneralSemantics<Transition>::instruction(); }




}
