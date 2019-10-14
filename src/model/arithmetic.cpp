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

#include "arithmetic.h"
#include "error.h"

namespace gologpp {


ArithmeticOperation::ArithmeticOperation(
	Expression *lhs,
	Operator op,
	Expression *rhs
)
: lhs_(lhs)
, rhs_(rhs)
, operator_(op)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}


const Expression &ArithmeticOperation::lhs() const
{ return *lhs_; }

const Expression &ArithmeticOperation::rhs() const
{ return *rhs_; }

ArithmeticOperation::Operator ArithmeticOperation::op() const
{ return operator_; }


string ArithmeticOperation::to_string(const string &pfx) const
{ return lhs().to_string(pfx) + " " + to_string(op()) + " " + rhs().to_string(pfx); }


}
