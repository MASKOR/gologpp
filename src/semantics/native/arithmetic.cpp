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

#include <model/arithmetic.h>
#include <model/value.h>

#include "arithmetic.h"
#include "semantics.h"

namespace gologpp {


template<>
Value Semantics<ArithmeticOperation>::evaluate(const BindingChain &b, const History &h)
{
	switch (element().op()) {
	case ArithmeticOperation::Operator::POWER:
		return element().lhs().semantics<Expression>().evaluate(b, h).pow(
			element().lhs().semantics<Expression>().evaluate(b, h)
		);
	}
}



}
