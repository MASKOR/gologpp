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
#include "plan.h"

#include <model/scope.h>
#include <model/platform/semantics.h>
#include <model/platform/component.h>
#include <model/platform/constraint.h>
#include <execution/controller.h>

namespace gologpp {


PlanTransformation::~PlanTransformation()
{}

void PlanTransformation::init(AExecutionContext &ctx)
{
	for (auto &c : global_scope().local_identifiers<platform::Component>()) {
		c->attach_semantics(ctx.semantics_factory());
		c->initialize(ctx);
		components.emplace_back(c);
	}
	for (auto &c : global_scope().constraints()) {
		c->attach_semantics(ctx.semantics_factory());
		constraints.emplace_back(*c);
	}
}



unique_ptr<Plan> DummyPlanTransformation::transform(Plan &&p)
{ return unique_ptr<Plan>(new Plan(std::move(p))); }


}
