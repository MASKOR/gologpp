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

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

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


}


#endif // GOLOGPP_ARITHMETIC_H_
