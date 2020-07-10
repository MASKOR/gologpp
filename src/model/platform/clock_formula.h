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

#pragma once

#include <model/language.h>
#include <model/utilities.h>
#include <model/gologpp.h>
#include <model/expressions.h>
#include <model/types.h>
#include <model/reference.h>

#include "component.h"
#include "semantics.h"


namespace gologpp {


template<>
class Reference<platform::Clock>
: public ZeroArityReference<platform::Clock>
{
public:
	using ZeroArityReference<platform::Clock>::ZeroArityReference;
};



namespace platform {



class ClockFormula
: public virtual AbstractLanguageElement
, public NoScopeOwner
{
protected:
	ClockFormula();
	ClockFormula(const ClockFormula &) = delete;
	ClockFormula(ClockFormula &&) = delete;
	ClockFormula &operator = (const ClockFormula &) = delete;
	ClockFormula &operator = (ClockFormula &&) = delete;

public:
	using ElementType = ClockFormula;

	virtual ~ClockFormula() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	ModelElement *parent();
	const ModelElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);
	virtual bool operator <= (const Type &type) const;

	template<class GologT = ClockFormula>
	GeneralSemantics<GologT> &general_semantics() const
	{ return ModelElement::general_semantics<GologT>(); }

	template<class GologT = ClockFormula>
	Semantics<GologT> &elem_semantics() const
	{ return ModelElement::semantics<GologT>(); }

protected:
	AbstractLanguageElement *parent_;
};




class ClockBound
: public ClockFormula
, public LanguageElement<ClockBound, BoolType>
{
public:
	enum class Operator {
		GE, GT, LE, LT
	};

	ClockBound(Reference<Clock> *clock, Operator op, Value *bound);

	Operator op() const;
	const Reference<Clock> &clock() const;
	const Value &bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*clock_, bound_)

private:
	unique_ptr<Reference<Clock>> clock_;
	Value bound_;
};


class BooleanClockOperation
: public ClockFormula
, public LanguageElement<BooleanOperation, BoolType>
{
public:
	enum class Operator {
		AND, OR
	};

	BooleanClockOperation(ClockFormula *lhs, Operator op, ClockFormula *rhs);

	Operator op() const;
	const ClockFormula &lhs() const;
	const ClockFormula &rhs() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<ClockFormula> lhs_, rhs_;
	Operator op_;
};



} // namespace platform



template<>
class GeneralSemantics<platform::ClockFormula>
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics() = default;

	virtual const platform::ClockFormula &clock_formula() const = 0;
};



} // namespace gologpp
