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

#include "value.h"
#include "activity.h"
#include "reference.h"
#include "execution.h"

#include <model/activity.h>

namespace gologpp {


const Activity &Semantics<Activity>::activity()
{ return dynamic_cast<const Activity &>(element()); }


EC_word Semantics<Activity>::plterm()
{
	return ::term(EC_functor("exog_state_change", 2),
		reference_term(element()),
		EC_atom(to_string(element().state()).c_str())
	);
}


EC_word Semantics<Activity>::sensing_result()
{
	return ::term(EC_functor("e", 2),
		activity().target()->senses()->semantics().plterm(),
		activity().sensing_result()->semantics().plterm()
	);
}



} // namespace gologpp
