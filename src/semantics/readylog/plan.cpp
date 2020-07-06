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

#include <execution/transition.h>
#include <model/procedural.h>

#include "plan.h"
#include "execution.h"
#include "transition.h"
#include "utilities.h"
#include "procedural.h"

namespace gologpp {



unique_ptr<Plan> plan_from_ec_word(EC_word policy)
{
	unique_ptr<Plan> rv(new Plan());

	EC_word list;
	if (policy.arg(1, list) != EC_succeed)
		throw Bug("Unexpected policy: " + ReadylogContext::instance().to_string(policy));

	EC_word head;
	while(list.is_nil() != EC_succeed && list.is_list(head, list) == EC_succeed) {
		shared_ptr<Transition> curr_action = Semantics<Transition>::transition_from_plterm(head);

		if (curr_action)
			rv->append(new Transition(*curr_action));

		else if (functor_name(head) == "marker") {

			Test *marker = new Test(new Value(get_type<BoolType>(), false));
			marker->attach_semantics(ReadylogContext::instance().semantics_factory());

			EC_word cond, value;
			EC_atom v_atom;
			if (head.arg(1, cond) != EC_succeed
				|| head.arg(2, value) != EC_succeed
				|| value.is_atom(&v_atom) != EC_succeed
			)
				throw Bug("Unexpected marker: " + ReadylogContext::instance().to_string(head));

			if (v_atom == EC_atom("fail") || v_atom == EC_atom("false"))
				cond = ::term(EC_functor("not", 1), cond);

			marker->semantics().make_plan_marker(cond);
			rv->append(marker);
		}
	}

	return rv;
}


}
