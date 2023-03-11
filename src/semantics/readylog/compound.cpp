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

#include "compound.h"
#include "list.h"
#include "reference.h"

namespace gologpp {


template<>
EC_word Semantics<CompoundExpression>::plterm()
{
	EC_word field_list = ::nil();
	for (auto &field_name : element().compound_type().field_names())
		field_list = ::list(
			::term(EC_functor(("#" + field_name).c_str(), 1),
				element().entry(field_name).semantics().plterm()
			),
			field_list
		);
	return ::term(EC_functor("gpp_compound", 2),
		EC_atom(("#" + element().type().name()).c_str()),
		field_list
	);
}


/**
 * Transform nested field- and list accesses into a pair of the innermost subject (must be a
 * list- or compound-valued fluent) and a sequence of list indices and field names. Only one of either
 * @a fa or @a la may be set, depending on whether the outermost expression is a list access or a field access.
 *
 * @param fa The rightmost field access or nullptr
 * @param la The rightmost list access or nullptr
 * @return A reference to the fluent (either list- or compound-valued), and an eclipse list
 *         with a mixture of list indices and field names that sequentially index (deeply) into the
 *         returned fluent.
 */
std::pair<const Reference<Fluent> *, EC_word> traverse_mixed_field_access(const FieldAccess *fa, const ListAccess *la) {
	const Expression *sub;
	EC_word field_list = ::nil();

	do {
		if (fa) {
			field_list = ::list(fa->special_semantics().pl_field_name(), field_list);
			sub = &fa->subject();
		}
		else if (la) {
			field_list = ::list(la->special_semantics().pl_index(), field_list);
			sub = &la->subject();
		}
		else
			throw Bug("Invalid FieldAccess statement: " + fa->str());

		fa = dynamic_cast<const FieldAccess *>(sub);
		la = dynamic_cast<const ListAccess *>(sub);
	} while (sub->is_a<FieldAccess>() || sub->is_a<ListAccess>());

	return { dynamic_cast<const Reference<Fluent> *>(sub), std::move(field_list) };
};



EC_word Semantics<FieldAccess>::plterm()
{
	return ::term(EC_functor("gpp_field_value", 2),
		pl_field_name(),
		element().subject().semantics().plterm()
	);
}


EC_atom Semantics<FieldAccess>::pl_field_name()
{ return EC_atom(("#" + element().field_name()).c_str()); }


EC_word Semantics<FieldAccess>::field_assign(const Expression &value)
{
	return ::term(EC_functor("gpp_field_assign", 3),
		pl_field_name(),
		value.semantics().plterm(),
		element().subject().semantics().plterm()
	);
}




} // namespace gologpp
