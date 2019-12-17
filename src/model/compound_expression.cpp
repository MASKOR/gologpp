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
#include <boost/fusion/include/at_c.hpp>

namespace gologpp {


CompoundExpression::CompoundExpression(
	const Type &t,
	const vector<fusion_wtf_vector<string, Expression *> > &entries
)
{
	set_type(t);

	if (!AbstractLanguageElement::type().is_compound())
		throw TypeError("Attempt to construct CompoundExpression, but type name \""
			+ type().name() + "\" does not refer to a compound type");

	std::unordered_set<string> unassigned_fields = compound_type().field_names();

	for (const auto &entry : entries) {
		const string &field_name = boost::fusion::at_c<0>(entry);
		const Type &field_type = compound_type().field_type(field_name);
		Expression *expr = boost::fusion::at_c<1>(entry);
		if (field_type >= expr->type())
			entries_.emplace(field_name, expr);
		else
			throw ExpressionTypeMismatch("Cannot assign " + expr->str()
				+ " to a field of type " + field_type.name()
			);
		unassigned_fields.erase(field_name);
	}

	string missing_fields;
	for (const string &s : unassigned_fields)
		missing_fields += s + ", ";
	if (missing_fields.size())
		missing_fields = missing_fields.substr(0, missing_fields.length() - 2);

	if (missing_fields.length())
		throw TypeError("CompoundExpression " + str()
			+ " is missing fields " + missing_fields
		);
}


const Expression &CompoundExpression::entry(const string &key) const
{
	auto it = entries_.find(key);
	if (it == entries_.end())
		throw std::logic_error("No key " + key + " in " + str());
	return *it->second;
}


void CompoundExpression::attach_semantics(SemanticsFactory &f)
{
	if (semantics_)
		return;

	for (auto &pair : entries_)
		pair.second->attach_semantics(f);
	set_semantics(f.make_semantics(*this));
}


string CompoundExpression::to_string(const string &pfx) const
{
	string rv = pfx + "{" linesep;
	for (const auto &pair : entries_)
		rv += pfx + indent + pair.first + " = " + pair.second->to_string("") + ", " linesep;
	return rv + pfx + "}" linesep;
}


const CompoundType &CompoundExpression::compound_type() const
{
	return dynamic_cast<const CompoundType &>(
		AbstractLanguageElement::type()
	);
}


}
