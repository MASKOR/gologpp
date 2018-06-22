#include "atoms.h"

namespace gologpp {

Implementation<AbstractVariable>::Implementation(const AbstractVariable &)
{}

Implementation<AbstractVariable>::~Implementation()
{}

void Implementation<AbstractVariable>::init()
{ ec_var_ = ::newvar(); }

EC_word Implementation<AbstractVariable>::term()
{ return ec_var_; }


template<>
EC_word Implementation<Constant<BooleanExpression>>::term()
{
	if (value_.representation() == "true")
		return EC_atom("true");
	else if (value_.representation() == "false")
		return EC_atom("fail");
	else
		throw Bug("Invalid boolean value `" + value_.representation() + "'");
}


template<>
EC_word Implementation<Constant<NumericExpression>>::term()
{
	if (value_.representation().find('.') != string::npos)
		return EC_word(std::stod(value_.representation()));
	else
		return EC_word(std::stol(value_.representation()));
}


}
