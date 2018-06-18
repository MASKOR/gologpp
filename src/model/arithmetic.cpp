#include "arithmetic.h"
#include "error.h"

namespace gologpp {


ArithmeticOperation::ArithmeticOperation(NumericExpression *lhs, Operator op, NumericExpression *rhs)
: NumericExpression(lhs->parent_scope())
, lhs_(lhs)
, rhs_(rhs)
, operator_(op)
{
	if (&lhs->parent_scope() != &rhs->parent_scope())
		throw Bug("ArithmeticOperation scope mismatch");
}


const NumericExpression &ArithmeticOperation::lhs() const
{ return *lhs_; }

const NumericExpression &ArithmeticOperation::rhs() const
{ return *rhs_; }

ArithmeticOperation::Operator ArithmeticOperation::op() const
{ return operator_; }


}