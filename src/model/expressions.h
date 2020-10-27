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

#include "language.h"
#include "utilities.h"
#include "gologpp.h"
#include "types.h"

#include <memory>
#include <algorithm>

namespace gologpp {



class Expression
: public ChildElement {
protected:
	Expression();
	Expression(const Expression &) = delete;
	Expression(Expression &&) = default;
	Expression &operator = (const Expression &) = delete;

public:
	using ElementType = Expression;

	virtual ~Expression() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	virtual bool operator <= (const Type &type) const;

	virtual bool operator == (const Expression &other) const;
	virtual bool operator != (const Expression &other) const;

	template<class GologT = Expression>
	GeneralSemantics<GologT> &general_semantics() const
	{ return ModelElement::general_semantics<GologT>(); }

	template<class GologT = Expression>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }
};



template<>
class GeneralSemantics<Expression>
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics() = default;
	virtual Value evaluate(const BindingChain &b, const History &h) = 0;

	virtual const Expression &expression() const = 0;
};



class Instruction
: public ChildElement {
protected:
	Instruction();
	Instruction(const Instruction &) = delete;
	Instruction(Instruction &&) = default;
	Instruction &operator = (const Instruction &) = delete;

public:
	using ElementType = Instruction;

	virtual ~Instruction() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	template<class GologT = Instruction>
	GeneralSemantics<GologT> &general_semantics() const
	{ return ModelElement::general_semantics<GologT>(); }

	template<class GologT = Instruction>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }
};



template<>
class GeneralSemantics<Instruction>
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics() = default;
	virtual bool final(const BindingChain &b, const History &h) = 0;

	/**
	 * @param b Binding that gives a value to all child variables that aren't bound by a pick statement
	 * @param h Current history (i.e. the current situation)
	 * @return Protocol: nullptr = not possible. Empty Plan = final. Non-empty plan = Execute plan and re-execute trans if not final.
	 */
	virtual unique_ptr<Plan> trans(const BindingChain &b, History &h) = 0;

	virtual const Instruction &instruction() const = 0;
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
