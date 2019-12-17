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

#ifndef GOLOGPP_COMPOUND_EXPRESSION_H_
#define GOLOGPP_COMPOUND_EXPRESSION_H_

#include "language.h"
#include "gologpp.h"
#include "expressions.h"
#include "scope.h"
#include "types.h"

#include <unordered_map>

namespace gologpp {


class CompoundExpression
: public Expression
, public LanguageElement<CompoundExpression>
, public NoScopeOwner
{
public:
	CompoundExpression(const Type &type, const vector<fusion_wtf_vector<string, Expression *>> &entries);

	const Expression &entry(const string &key) const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	const CompoundType &compound_type() const;

private:
	std::unordered_map<string, unique_ptr<Expression>> entries_;
};



} // namespace gologpp

#endif // GOLOGPP_COMPOUND_EXPRESSION_H_
