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

#include "reference.h"
#include "variable.h"
#include "history.h"
#include "execution.h"
#include "formula.h"
#include "action.h"
#include "history.h"

#include <execution/plan.h>
#include <execution/controller.h>
#include <execution/activity.h>


namespace gologpp {


template<>
unique_ptr<Plan> Semantics<Reference<Action>>::trans(const BindingChain &b, History &h)
{
	vector<unique_ptr<Expression>> ground_args;
	for (const Expression *e : element().args())
		ground_args.emplace_back(unique_ptr<Expression>(e->semantics().evaluate(b, h).copy()));

	try {
		Transition trans_start(element().target(), ground_args, Transition::Hook::START);
		trans_start.attach_semantics(context().semantics_factory());
		trans_start.semantics().trans({}, h);

		Transition trans_end(element().target(), ground_args, Transition::Hook::END);
		trans_end.attach_semantics(context().semantics_factory());
		trans_end.semantics().trans({}, h);
	} catch (Escalate &e) {
		e.set_static_cause(&element());
		throw;
	}

	return nullptr;
}



template<>
unique_ptr<Plan> Semantics<Transition>::trans(const BindingChain &b, History &h)
{
	if (b.size())
		throw Bug("Attempting to execute transition with non-empty binding");

	shared_ptr<Activity> a;
	switch (element().hook()) {
	case Transition::Hook::START:
		context().backend().start_activity(element());
		break;
	case Transition::Hook::CANCEL:
		context().backend().cancel_activity(element());
		break;
	case Transition::Hook::END:
		context().backend().end_activity(element());
		break;
	case Transition::Hook::FINISH:
		a = context().backend().end_activity(element());
		if (a->state() != Activity::target_state(element().hook()))
			throw Escalate(nullptr, element().shared_from_this());
		break;
	case Transition::Hook::FAIL:
		throw Unsupported(element().str() + ": " + to_string(element().hook()) + " not supported");
	}

	h.special_semantics().append(element());
	return nullptr;
}



} // namespace gologpp
