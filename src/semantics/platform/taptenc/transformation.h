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

#pragma once

#include <model/gologpp.h>
#include <execution/transformation.h>
#include <execution/plan.h>

#include <taptenc/src/transformation.h>

namespace gologpp {


class TaptencTransformation : public PlanTransformation {
public:
	virtual void init(AExecutionController &ctx) override;
	virtual unique_ptr<Plan> transform(Plan &&) override;

private:
	AExecutionController &context();

	shared_ptr<const platform::Component> find_component_ref(const platform::StateSpec &constraint_expr);
	vector<taptenc::PlanAction> plan_gpp_to_taptenc(Plan &&);
	unique_ptr<Plan> plan_taptenc_to_gpp(taptenc::timed_trace_t &&);

	std::string store_arg(const Value &);
	Value *retrieve_arg(std::string taptenc_symbolic_arg);

	TimedInstruction parse_domain_action(const std::string &tt_action, taptenc::groundedActionTime tt_time);
	TimedInstruction parse_platform_action(const std::string &tt_action, taptenc::groundedActionTime tt_time);

	void tt_update_automata();

	std::vector<taptenc::Automaton> tt_automata_;
	taptenc::transformation::Constraints tt_constraints_;

	AExecutionController *context_;

	vector<Value *> arg_storage_;

	std::unordered_map <
		shared_ptr<const platform::Component>,
		vector<std::reference_wrapper<platform::Constraint>>
	> platform_model_;

};


}
