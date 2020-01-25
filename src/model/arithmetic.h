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

#ifndef GOLOGPP_ARITHMETIC_H_
#define GOLOGPP_ARITHMETIC_H_

#include "expressions.h"
#include "language.h"
#include "scope.h"

#include <iostream>

namespace gologpp {


class ArithmeticOperation : public Expression, public NoScopeOwner, public LanguageElement<ArithmeticOperation, NumberType> {
public:
	enum Operator {
		ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, POWER, MODULO
	};
	ArithmeticOperation(Expression *lhs, Operator op, Expression *rhs);

	const Expression &lhs() const;
	const Expression &rhs() const;
	Operator op() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	static std::string to_string(ArithmeticOperation::Operator op)
	{
		switch (op) {
		case ADDITION:
			return "+";
		case SUBTRACTION:
			return "-";
		case MULTIPLICATION:
			return "*";
		case DIVISION:
			return "/";
		case POWER:
			return "^";
		case MODULO:
			return "%";
		}

		return "[Unknown ArithmeticOperation]";
	}

	virtual string to_string(const string &pfx) const override;

private:
	SafeExprOwner<NumberType> lhs_, rhs_;
	Operator operator_;
};

using ArithmeticOperator = ArithmeticOperation::Operator;


constexpr unsigned int precedence(ArithmeticOperation::Operator op) {
	switch (op) {
	case ArithmeticOperator::ADDITION:
	case ArithmeticOperator::SUBTRACTION:
		return 1;
	case ArithmeticOperator::MULTIPLICATION:
	case ArithmeticOperator::DIVISION:
	case ArithmeticOperator::MODULO:
		return 2;
	case ArithmeticOperator::POWER:
		return 3;
	}
	throw Bug("Undefined arithmetic operator: " + std::to_string(op));
}



}


#endif // GOLOGPP_ARITHMETIC_H_
