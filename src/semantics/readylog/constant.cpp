#include "constant.h"
#include <model/constant.h>

namespace gologpp {

template<>
EC_word Semantics<Constant<BooleanExpression>>::plterm()
{
	if (value_.representation<bool>())
		return EC_atom("true");
	else
		return EC_atom("fail");
}


template<>
EC_word Semantics<Constant<NumericExpression>>::plterm()
{
	if (value_.variant().type() == boost::typeindex::type_id<double>())
		return EC_word(value_.representation<double>());
	else if (value_.variant().type() == boost::typeindex::type_id<int>())
		return EC_word(value_.representation<int>());
	else
		throw std::runtime_error("Unknown Constant type");
}


template<>
EC_word Semantics<Constant<SymbolicExpression>>::plterm()
{ return EC_atom(value_.str().c_str()); }


template<>
EC_word Semantics<Constant<StringExpression>>::plterm()
{ return EC_atom(value_.str().c_str()); }


} // namespace gologpp
