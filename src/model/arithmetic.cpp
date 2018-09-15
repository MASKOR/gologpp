#include "arithmetic.h"
#include "error.h"

namespace gologpp {


ArithmeticOperation::ArithmeticOperation(
	NumericExpression *lhs,
	Operator op,
	NumericExpression *rhs
)
: lhs_(lhs)
, rhs_(rhs)
, operator_(op)
{
	if (&lhs->parent_scope() != &rhs->parent_scope())
		throw Bug("ArithmeticOperation scope mismatch");

	lhs_->set_parent(this);
	rhs_->set_parent(this);
}


const NumericExpression &ArithmeticOperation::lhs() const
{ return *lhs_; }

const NumericExpression &ArithmeticOperation::rhs() const
{ return *rhs_; }

ArithmeticOperation::Operator ArithmeticOperation::op() const
{ return operator_; }


string ArithmeticOperation::to_string(const string &pfx) const
{ return lhs().to_string(pfx) + " " + to_string(op()) + " " + rhs().to_string(pfx); }


}
