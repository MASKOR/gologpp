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

#include <execution/context.h>


namespace gologpp {


unique_ptr<Plan> Semantics<Reference<Action>>::trans(const Binding &b, History &h) {
}


unique_ptr<Plan> Semantics<Reference<Action>>::plan(const Binding &b, History &h) {
	Binding merged = element().binding().ground(b);
	if (element()->precondition().general_semantics().evaluate(merged, h) != Value(get_type<BoolType>(), true))
		throw NotPossible();
}


} // namespace gologpp
