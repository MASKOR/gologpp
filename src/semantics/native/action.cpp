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

#include <execution/activity.h>

#include "action.h"
#include "effect_axiom.h"
#include "scope.h"
#include "execution.h"
#include "variable.h"
#include "value.h"

#include <unordered_map>

namespace gologpp {


template<>
unique_ptr<Plan> Semantics<Transition>::trans(const Binding &b, History &h) {
	while (!element()->precondition().semantics<Expression>().evaluate(b, h))
		this->context().drain_exog_queue_blocking();

	vector<unique_ptr<Value>> arg_vals;
	Binding b1(b);
	for (const auto &param : element()->params()) {
		Value v = element().arg_for_param(param).semantics<Expression>().evaluate(b, h);
		arg_vals.emplace_back(new Value(
			param->type(),
			v
		));
		// TODO: Meh. Dangerous object lifetime here.
		b1.bind(param, *arg_vals.back());
	}

	shared_ptr<Transition> trans_start = std::make_shared<Transition>(
		element()->shared_from_this(),
		arg_vals,
		Transition::Hook::START
	);

	// TODO: eliminate Transition object here
	shared_ptr<Activity> a = context().backend().start_activity(*trans_start);
	h.semantics().append(trans_start);
	context().backend().wait_for_end(*a);
}



} // namespace gologpp
