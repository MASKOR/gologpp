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
#include <execution/context.h>

#include <taptenc/src/transformation.h>

namespace gologpp {


void TaptencTransformation::init(AExecutionContext &ctx)
{
	context_ = &ctx;

	std::unordered_map <
		shared_ptr<const platform::Component>,
		vector<std::reference_wrapper<platform::Constraint>>
	> platform_model;

	for (const auto &constraint : global_scope().constraints())
		platform_model[find_component_ref(constraint->rhs())]
		              .push_back(std::ref(*constraint));

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


shared_ptr<const platform::Component> TaptencTransformation::find_component_ref(const Expression &expr)
{
	if (expr.is_a<platform::StateAssertion>())
		return expr.cast<platform::StateAssertion>().component().target();
	else if (expr.is_a<platform::TemporalBinaryOperation>()) {
		shared_ptr<const platform::Component> lhs_ref = find_component_ref(expr.cast<platform::TemporalBinaryOperation>().lhs());
		shared_ptr<const platform::Component> rhs_ref = find_component_ref(expr.cast<platform::TemporalBinaryOperation>().rhs());
		if (lhs_ref != rhs_ref)
			throw Unsupported("Taptenc semantics require that a state spec refer to one component only: " + expr.str());
		return lhs_ref;
	}
	else if (expr.is_a<platform::BooleanConstraintOperation>()) {
		shared_ptr<const platform::Component> lhs_ref = find_component_ref(expr.cast<platform::BooleanConstraintOperation>().lhs());
		shared_ptr<const platform::Component> rhs_ref = find_component_ref(expr.cast<platform::BooleanConstraintOperation>().rhs());
		if (lhs_ref != rhs_ref)
			throw Unsupported("Taptenc semantics require that a state spec refer to one component only: " + expr.str());
		return lhs_ref;
	}
	else if (expr.is_a<platform::TemporalUnaryOperation>())
		return find_component_ref(expr.cast<platform::TemporalUnaryOperation>().subject());
	else
		throw Unsupported("Unexpected constraint expression: " + expr.str());
}


vector<taptenc::PlanAction> TaptencTransformation::plan_gpp_to_taptenc(Plan &&p)
{
	vector<taptenc::PlanAction> rv;
	for (auto &ti : p.elements()) {
		try {
			Transition &trans = ti.instruction().cast<Transition>();
			std::vector<std::string> argstr;
			for (auto &arg : trans.args())
				argstr.push_back(arg->str());
			rv.push_back(taptenc::PlanAction {
				taptenc::ActionName(trans->name(), argstr),
				taptenc::Bounds(
					boost::numeric_cast<taptenc::timepoint>(
						(ti.earliest_timepoint() - context().context_time()).count()
					),
					boost::numeric_cast<taptenc::timepoint>(
						(ti.latest_timepoint() - context().context_time()).count()
					)
				),
				taptenc::Bounds(0, std::numeric_limits<taptenc::timepoint>::max())
			} );
		} catch (std::bad_cast &)
		{
			throw Unsupported("Non-transition element found in plan: " + ti.instruction().str());
		}
	}
	return rv;
}


unique_ptr<Plan> TaptencTransformation::plan_taptenc_to_gpp(taptenc::timed_trace_t &&)
{
	// TODO
}



} // namespace gologpp
