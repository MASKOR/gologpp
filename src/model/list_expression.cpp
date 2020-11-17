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

#include "list_expression.h"
#include "semantics.h"

namespace gologpp {


ListExpression::ListExpression(
	const Type &t,
	const boost::optional<vector<Expression *>> &entries
)
{
	set_type_unchecked(t);

	if (!AbstractLanguageElement::type().is<ListType>())
		throw TypeError("Attempt to construct ListExpression, but type name \""
			+ t.name() + "\" does not refer to a list type");

	for (Expression *expr : entries.get_value_or({})) {
		if (type().element_type() >= *expr)
			entries_.emplace_back(expr);
		else
			throw ExpressionTypeMismatch("Cannot assign " + expr->str()
				+ " to a " + type().name()
			);
	}
}


const Expression &ListExpression::entry(size_t idx) const
{
	if (idx >= entries_.size())
		throw UserError(str() + ": Index " + std::to_string(idx) + " out of range ");
	return *entries_[idx];
}

const vector<unique_ptr<Expression> > &ListExpression::entries() const
{ return entries_; }

size_t ListExpression::size() const
{ return entries_.size(); }


void ListExpression::attach_semantics(SemanticsFactory &f)
{
	if (semantics_)
		return;

	for (auto &expr : entries_)
		expr->attach_semantics(f);
	set_semantics(f.make_semantics(*this));
}


string ListExpression::to_string(const string &pfx) const
{
	string rv = pfx + "[";
	for (const auto &elem : entries_)
		rv += elem->str() + ", ";
	if (entries_.size() > 0)
		rv = rv.substr(0, rv.length() - 2);
	return rv + "]";
}


const ListType &ListExpression::type() const
{
	return dynamic_cast<const ListType &>(
		AbstractLanguageElement::type()
	);
}


bool ListExpression::operator <= (const Type &t) const
{
	try {
		const ListType &lt = dynamic_cast<const ListType &>(t);
		for (const auto &entry : entries())
			if (!(*entry <= lt.element_type()))
				return false;
		return true;
	} catch (std::bad_cast &) {}

	return t >= *this;
}


} // namespace gologpp
