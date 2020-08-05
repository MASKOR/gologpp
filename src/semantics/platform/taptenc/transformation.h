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

#include <taptenc/src/transformation.h>

namespace gologpp {


class TaptencTransformation : public PlanTransformation {
public:
	TaptencTransformation();

	virtual unique_ptr<Plan> transform(Plan &&) override;

private:
	shared_ptr<platform::Component> find_component_ref(const Expression &constraint_expr);
	vector<taptenc::PlanAction> plan_gpp_to_taptenc(Plan &&);
	unique_ptr<Plan> plan_taptenc_to_gpp(taptenc::timed_trace_t &&);

	std::vector<taptenc::Automaton> tt_automata_;
	taptenc::transformation::Constraints tt_constraints_;
};


}
