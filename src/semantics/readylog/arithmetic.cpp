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
#include <model/arithmetic.h>

namespace gologpp {


Semantics<ArithmeticOperation>::Semantics(const ArithmeticOperation &op)
: operation_(op)
{
	switch (operation_.op()) {
	case ArithmeticOperation::ADDITION:
		functor_ = "+";
		break;
	case ArithmeticOperation::SUBTRACTION:
		functor_ = "-";
		break;
	case ArithmeticOperation::MULTIPLICATION:
		functor_ = "*";
		break;
	case ArithmeticOperation::DIVISION:
		functor_ = "/";
		break;
	case ArithmeticOperation::POWER:
		functor_ = "^";
		break;
	case ArithmeticOperation::MODULO:
		functor_ = "mod";
		break;
	}
}


EC_word Semantics<ArithmeticOperation>::plterm()
{
	return ::term(EC_functor(functor_, 2),
		operation_.lhs().semantics().plterm(),
		operation_.rhs().semantics().plterm()
	);
}


}
