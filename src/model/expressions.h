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

#ifndef GOLOGPP_EXPRESSIONS_H_
#define GOLOGPP_EXPRESSIONS_H_

#include "model_element.h"
#include "utilities.h"
#include "gologpp.h"
#include "types.h"

#include <memory>
#include <algorithm>

namespace gologpp {



class Expression : public virtual AbstractLanguageElement {
protected:
	Expression();
	Expression(const Expression &) = delete;
	Expression(Expression &&) = default;
	Expression &operator = (const Expression &) = delete;

public:
	virtual ~Expression() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	AbstractLanguageElement *parent();
	const AbstractLanguageElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);
	virtual bool operator <= (const Type &type) const;

protected:
	AbstractLanguageElement *parent_;
};



class Instruction : public virtual AbstractLanguageElement {
protected:
	Instruction();
	Instruction(const Instruction &) = delete;
	Instruction(Instruction &&) = default;
	Instruction &operator = (const Instruction &) = delete;

public:
	virtual ~Instruction() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	AbstractLanguageElement *parent();
	const AbstractLanguageElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);

protected:
	AbstractLanguageElement *parent_;
};



template<class T>
class SafeExprOwner : public std::unique_ptr<Expression> {
public:
	using std::unique_ptr<Expression>::unique_ptr;

	SafeExprOwner()
	: std::unique_ptr<Expression>()
	{}

	SafeExprOwner(Expression *e)
	: std::unique_ptr<Expression>(e)
	{ e->ensure_type<T>(); }

	SafeExprOwner<T> &operator = (Expression *e)
	{
		e->ensure_type<T>();
		std::unique_ptr<Expression>::operator = (std::unique_ptr<Expression>(e));
		return *this;
	}
};


template<class T>
using SafeVector = vector<SafeExprOwner<T>>;


template<class T>
string to_string(const SafeExprOwner<T> &o, const string &pfx)
{ return o->to_string(pfx); }



}

#endif // GOLOGPP_EXPRESSIONS_H_
