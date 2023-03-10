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

#ifndef GOLOGPP_LIST_EXPRESSION_H_
#define GOLOGPP_LIST_EXPRESSION_H_

#include "language.h"
#include "gologpp.h"
#include "expressions.h"
#include "scope.h"
#include "types.h"

#include <unordered_map>

#include <boost/optional.hpp>

namespace gologpp {


class ListExpression
: public Expression
, public LanguageElement<ListExpression>
, public NoScopeOwner
{
public:
	ListExpression(const Type &type, const boost::optional<vector<Expression *>> &entries);

	const Expression &entry(size_t idx) const;
	const vector<unique_ptr<Expression>> &entries() const;
	size_t size() const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual const ListType &type() const override;
	virtual bool operator <= (const Type &t) const override;

private:
	vector<unique_ptr<Expression>> entries_;
};



} // namespace gologpp

#endif // GOLOGPP_LIST_EXPRESSION_H_
