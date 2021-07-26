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

#include "scope.h"
#include "procedural.h"
#include "reference.h"
#include "execution.h"
#include "value.h"
#include "action.h"

#include <execution/plan.h>

#include <model/procedural.h>


namespace gologpp {


template<>
unique_ptr<Plan> Semantics<Block>::trans(const BindingChain &b, History &h)
{
	for (const unique_ptr<Instruction> &instr : element().elements())
		instr->semantics<Instruction>().trans(b, h);
}



}
