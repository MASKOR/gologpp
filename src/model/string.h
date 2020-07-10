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

#ifndef GOLOGPP_STRING_CONCATENATION_H_
#define GOLOGPP_STRING_CONCATENATION_H_

#include "expressions.h"
#include "language.h"
#include "scope.h"
#include "semantics.h"


namespace gologpp {


class ToString
: public Expression
, public NoScopeOwner
, public LanguageElement<ToString, StringType> {
public:
	ToString(Expression *expr);

	const Expression &expression() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*expr_)

	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Expression> expr_;
};



class StringConcatenation : public Expression, public NoScopeOwner, public LanguageElement<StringConcatenation, StringType> {
public:
	StringConcatenation(Expression *lhs, Expression *rhs);

	const Expression &rhs() const;
	const Expression &lhs() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

private:
	SafeExprOwner<StringType> lhs_, rhs_;
};



}

#endif /// GOLOGPP_STRING_CONCATENATION_H_
