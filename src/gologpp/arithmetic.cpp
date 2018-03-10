#include "arithmetic.h"
#include "error.h"

namespace gologpp {


ArithmeticOperation::ArithmeticOperation(unique_ptr<ValueExpression> &&lhs, Operator op, unique_ptr<ValueExpression> &&rhs)
: ValueExpression(lhs->parent_scope())
, lhs_(std::move(lhs))
, rhs_(std::move(rhs))
, operator_(op)
{
	if (&lhs->parent_scope() != &rhs->parent_scope())
		throw Bug("ArithmeticOperation scope mismatch");
}


const ValueExpression &ArithmeticOperation::lhs() const
{ return *lhs_; }

const ValueExpression &ArithmeticOperation::rhs() const
{ return *rhs_; }

ArithmeticOperation::Operator ArithmeticOperation::op() const
{ return operator_; }


}