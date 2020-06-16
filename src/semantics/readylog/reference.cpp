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
{ return element().target()->semantics().plterm(); }



template<>
EC_word Semantics<Reference<Action>>::plterm()
{
	return ::list(::term(EC_functor("start", 1), reference_term(element())),
		::list(::term(EC_functor("end", 1), reference_term(element())),
			::nil()
		)
	);
}


Semantics<TBinding<Value>> *Semantics<TBinding<Value>>::copy(const TBinding<Value> &target_element) const
{ return new Semantics<TBinding<Value>>(target_element, rl_context()); }

const TBinding<Value> &Semantics<TBinding<Value> >::model_element() const
{ return this->element(); }

void Semantics<TBinding<Value> >::init_vars()
{
	for (const auto &pval : this->element().map())
		pval.first->semantics().init();
}




} // namespace gologpp
