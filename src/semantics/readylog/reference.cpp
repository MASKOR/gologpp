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

namespace gologpp {


EC_word reference_term(const Reference<Variable> &ref)
{ return EC_atom(ref.name().c_str()); }

template<>
EC_word Semantics<Reference<Variable>>::plterm()
{ return element().target()->special_semantics().plterm(); }



template<>
EC_word Semantics<Reference<Action>>::plterm()
{
	return ::list(::term(EC_functor("start", 1), reference_term(element())),
		::list(::term(EC_functor("end", 1), reference_term(element())),
			::nil()
		)
	);
}


EC_word gologpp::Semantics<Binding>::plterm()
{
	if (this->element().map().empty())
		return EC_atom("true");
	else {
		vector<EC_word> pl_binds;
		for (const auto &pval : this->element().map()) {
			pl_binds.push_back(
				::term(EC_functor("=", 2),
					pval.first->special_semantics().plterm(),
					pval.second->semantics().plterm()
				)
			);
		}

		return to_ec_term(",", pl_binds);
	}
}

Semantics<Binding> *Semantics<Binding>::copy(const Binding &target_element) const
{ return new Semantics<Binding>(target_element, rl_context()); }

const Binding &Semantics<Binding >::model_element() const
{ return this->element(); }


{
	for (const auto &pval : this->element().map())
		pval.first->special_semantics().init();
}




} // namespace gologpp
