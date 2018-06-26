#ifndef GOLOGPP_ARITHMETIC_H_
#define GOLOGPP_ARITHMETIC_H_

#include "expressions.h"
#include "language.h"

namespace gologpp {


class ArithmeticOperation : public NumericExpression, public LanguageElement<ArithmeticOperation> {
public:
	enum Operator {
		ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, POWER, MODULO
	};
	ArithmeticOperation(NumericExpression *lhs, Operator op, NumericExpression *rhs);

	const NumericExpression &lhs() const;
	const NumericExpression &rhs() const;
	Operator op() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<NumericExpression> lhs_, rhs_;
	Operator operator_;
};


}

#endif // GOLOGPP_ARITHMETIC_H_
