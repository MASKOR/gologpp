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

#include "transformation.h"

#include <model/scope.h>
#include <model/platform/component.h>
#include <model/platform/constraint.h>

#include <semantics/platform/taptenc/component.h>
#include <semantics/platform/taptenc/constraint.h>

#include <execution/plan.h>

#include <taptenc/src/transformation.h>

namespace gologpp {


TaptencTransformation::TaptencTransformation()
{
	std::unordered_map <
		shared_ptr<platform::Component>,
		vector<std::reference_wrapper<platform::Constraint>>
	> platform_model;

	for (const auto &constraint : global_scope().constraints())
		platform_model[find_component_ref(constraint->rhs())].push_back(std::ref(*constraint));

	taptenc::transformation::Constraints::size_type i = 0;
	for (const auto &pair : platform_model) {
		tt_automata_.emplace_back(*pair.first->semantics().compile());
		for (const auto &constraint : pair.second)
			tt_constraints_[i].emplace_back(constraint.get().semantics().compile());
	}
}


unique_ptr<Plan> TaptencTransformation::transform(Plan &&p)
{
	return plan_taptenc_to_gpp(
		taptenc::transformation::transform_plan(
			plan_gpp_to_taptenc(std::move(p)),
			tt_automata_,
			tt_constraints_
		)
	);
}

shared_ptr<platform::Component> TaptencTransformation::find_component_ref(const Expression &constraint_expr)
{
	// TODO
}

vector<taptenc::PlanAction> TaptencTransformation::plan_gpp_to_taptenc(Plan &&)
{
	// TODO
}

unique_ptr<Plan> TaptencTransformation::plan_taptenc_to_gpp(taptenc::timed_trace_t &&)
{
	// TODO
}


}
