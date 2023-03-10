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

#include "list.h"

namespace gologpp {


template<>
EC_word Semantics<ListExpression>::plterm()
{
	EC_word list = ::nil();

	for (size_t i = element().size(); i > 0; --i)
		list = ::list(element().entry(i - 1).semantics().plterm(), list);

	return ::term(EC_functor("gpp_list", 2),
		EC_atom( (
			"#" + dynamic_cast<const ListType &>(element().type()).element_type().name()
		).c_str() ),
		list
	);
}


} // namespace gologpp
