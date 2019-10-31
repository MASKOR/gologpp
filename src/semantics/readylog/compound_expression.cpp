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

#include "compound_expression.h"

namespace gologpp {


Semantics<CompoundExpression>::Semantics(const CompoundExpression &expr)
: AbstractSemantics<CompoundExpression>(expr)
{}


EC_word Semantics<CompoundExpression>::plterm()
{
	EC_word field_list = ::nil();
	for (auto &field_name : element().type().field_names())
		field_list = ::list(
			::term(EC_functor(("#" + field_name).c_str(), 1),
				element().entry(field_name).semantics().plterm()
			),
			field_list
		);
	return ::term(EC_functor("gpp_compound", 2),
		EC_atom(("#" + element().type_name()).c_str()),
		field_list
	);
}


} // namespace gologpp
