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

#include "string.h"

namespace gologpp {


Semantics<ToString>::Semantics(const ToString &to_string)
: AbstractSemantics<ToString>(to_string)
{}


EC_word Semantics<ToString>::plterm()
{
	return ::term(EC_functor("to_string", 1),
		element().expression().semantics().plterm()
	);
}



Semantics<StringConcatenation>::Semantics(const StringConcatenation &subject)
: AbstractSemantics<StringConcatenation>(subject)
{}


EC_word Semantics<StringConcatenation>::plterm()
{
	return ::term(EC_functor("strcat", 2),
		element().lhs().semantics().plterm(),
		element().rhs().semantics().plterm()
	);
}


}
