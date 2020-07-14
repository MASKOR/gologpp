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
#include "variable.h"
#include "execution.h"

namespace gologpp {

Semantics<Scope>::Semantics(const Scope &s, ReadylogContext &context)
: GeneralSemantics<Scope>(s, context)
{}


EC_word Semantics<Scope>::plterm()
{ throw Bug("This method should not be called."); }


EC_word *Semantics<Scope>::variables(const vector<string> &names)
{
	EC_word *rv = new EC_word[names.size()];
	arity_t i = 0;
	for (const string &name : names)
		rv[i++] = element().lookup_var(name)->semantics().plterm();
	return rv;
}


void Semantics<Scope>::init_vars()
{
	for (auto &v : element().vars())
		if (&element() == &(global_scope()) || !element().parent_scope().lookup_var(v->name()))
			// Only init variables that aren't in the parent scope
			// (They are initialized there).
			v->semantics<Variable>().init();
}


} // namespace gologpp
