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


Semantics<Reference<Variable>>::Semantics(const Reference<Variable> &ref)
: AbstractSemantics<Reference<Variable>>(ref)
{}

EC_word Semantics<Reference<Variable>>::plterm()
{ return element().target()->semantics().plterm(); }



template<>
EC_word Semantics<ReferenceBase<Action, Expression>>::plterm()
{
	return ::list(::term(EC_functor("start", 2), reference_term(element()), EC_atom("now")),
		::list(::term(EC_functor("finish", 2), reference_term(element()), EC_atom("now")),
			::nil()
		)
	);
}

} // namespace gologpp
