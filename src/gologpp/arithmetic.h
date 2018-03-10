#ifndef GOLOGPP_ARITHMETIC_H_
#define GOLOGPP_ARITHMETIC_H_

#include "expressions.h"

namespace gologpp {


class ArithmeticOperation : public ValueExpression, public LanguageElement<ArithmeticOperation> {
public:
	enum Operator {
		ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, POWER
	};
	ArithmeticOperation(unique_ptr<ValueExpression> &&lhs, Operator op, unique_ptr<ValueExpression> &&rhs);

	const ValueExpression &lhs() const;
	const ValueExpression &rhs() const;
	Operator op() const;

	//DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

private:
	unique_ptr<ValueExpression> lhs_, rhs_;
	Operator operator_;
};


}

#endif // GOLOGPP_ARITHMETIC_H_
