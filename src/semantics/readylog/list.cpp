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



EC_word Semantics<ListAccess>::pl_index()
{ return element().index().semantics().plterm(); }


EC_word Semantics<ListAccess>::plterm()
{
	return ::term(EC_functor("gpp_list_access", 2),
		element().subject().semantics().plterm(),
		pl_index()
	);
}



template<>
EC_word Semantics<ListPop>::plterm()
{
	string fn;
	switch(element().which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_pop_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_pop_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(element().which_end()));

	return ::term(EC_functor("set", 2),
		element().list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 1),
			element().list().semantics().plterm()
		)
	);

}



template<>
EC_word Semantics<ListPush>::plterm()
{
	string fn;
	switch(element().which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_push_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_push_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(element().which_end()));

	return ::term(EC_functor("set", 2),
		element().list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 2),
			element().list().semantics().plterm(),
			element().what().semantics().plterm()
		)
	);
}



template<>
EC_word Semantics<ListLength>::plterm()
{
	return ::term(EC_functor("gpp_list_length", 1),
		element().subject().semantics().plterm()
	);
}



} // namespace gologpp
