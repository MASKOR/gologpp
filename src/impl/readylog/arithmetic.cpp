#include "arithmetic.h"
#include <model/arithmetic.h>

namespace gologpp {


Implementation<ArithmeticOperation>::Implementation(const ArithmeticOperation &op)
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


EC_word Implementation<ArithmeticOperation>::term()
{
	return ::term(EC_functor(functor_, 2),
		operation_.lhs().implementation().term(),
		operation_.rhs().implementation().term()
	);
}


}