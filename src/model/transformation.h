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

#include "gologpp.h"

namespace gologpp {



class Plan;


/**
 * @class PlanTransformation
 * Keep this abstract so we can have multiple implementations.
 */
class PlanTransformation {
public:
	virtual ~PlanTransformation();

	virtual unique_ptr<Plan> transform(const Plan &) = 0;
};



}
