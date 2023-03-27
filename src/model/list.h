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

#include "gologpp.h"
#include "language.h"
#include "expressions.h"
#include "scope.h"
#include "types.h"
#include "semantics.h"

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListAccess
: public Expression
, public NoScopeOwner
, public LanguageElement<ListAccess>
{
public:
	ListAccess(Expression *subject, Expression *index);
	const Expression &subject() const;
	const Expression &index() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_, *index_)

	virtual const Type &type() const override;

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> subject_;
	SafeExprOwner<NumberType> index_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListLength
: public Expression
, public NoScopeOwner
, public LanguageElement<ListLength, NumberType>
{
public:
	ListLength(Expression *subject);
	const Expression &subject() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> subject_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

enum ListOpEnd {
	FRONT, BACK
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListPop
: public Instruction
, public NoScopeOwner
, public LanguageElement<ListPop, VoidType>
{
public:
	ListPop(Expression *list, ListOpEnd which_end);
	const Expression &list() const;
	ListOpEnd which_end() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*list_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> list_;
	ListOpEnd which_end_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListPush
: public Instruction
, public NoScopeOwner
, public LanguageElement<ListPush, VoidType>
{
public:
	ListPush(Expression *list, ListOpEnd which_end, Expression *what);
	const Expression &list() const;
	ListOpEnd which_end() const;
	const Expression &what() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*list_, *what_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> list_;
	ListOpEnd which_end_;
	unique_ptr<Expression> what_;
};


} // namespace gologpp

#endif // GOLOGPP_LIST_EXPRESSION_H_
