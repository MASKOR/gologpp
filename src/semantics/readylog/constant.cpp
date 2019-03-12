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


template<>
EC_word Semantics<Constant<CompoundExpression>>::plterm()
{
	EC_word field_list = ::nil();
	for (auto &pair : value_.value())
		field_list = ::list(
			::term(EC_functor(pair.first.c_str(), 1),
				pair.second->semantics().plterm()
			),
			field_list
		);
	return ::term(EC_functor("gpp_compound", 1),
		field_list
	);
}


} // namespace gologpp
