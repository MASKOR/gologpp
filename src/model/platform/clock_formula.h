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

#include <model/platform/component.h>
#include <model/platform/semantics.h>
#include <model/platform/reference.h>


namespace gologpp {
namespace platform {


class ClockFormula
: public virtual Expression
, public virtual AbstractLanguageElement
, public NoScopeOwner
{
public:
	template<class = ClockFormula>
	Semantics<ClockFormula> &semantics() const
	{ return Expression::semantics<ClockFormula>(); }
};


class ClockBound
: public ClockFormula
, public LanguageElement<ClockBound, BoolType>
{
public:
	enum class Operator {
		GE, LE
	};

	ClockBound(Reference<Clock> *clock, Operator op, Value *bound);

	Operator op() const;
	const Reference<Clock> &clock() const;
	const Value &bound() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*clock_, *bound_)

private:
	unique_ptr<Reference<Clock>> clock_;
	Operator op_;
	unique_ptr<Value> bound_;
};

string to_string(ClockBound::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

string to_string(BooleanClockOperation::Operator op);

unsigned int precedence(BooleanClockOperation::Operator op);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ClockNegation
: public ClockFormula
, public LanguageElement<ClockNegation, BoolType>
{
public:
	ClockNegation(ClockFormula *subject);

	const ClockFormula &subject() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_)

private:
	unique_ptr<ClockFormula> subject_;
};


} // namespace platform

} // namespace gologpp
