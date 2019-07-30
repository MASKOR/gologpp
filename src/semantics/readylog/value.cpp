#include "value.h"
#include <model/value.h>

namespace gologpp {

Semantics<Value>::Semantics(const Value &value)
: value_(value)
{}

EC_word Semantics<Value>::plterm()
{
	if (value_.type().is<BoolType>()) {
		if (static_cast<bool>(value_))
			return EC_atom("true");
		else
			return EC_atom("fail");
	}
	else if (value_.type().is<NumberType>()) {
		if (value_.variant().type() == boost::typeindex::type_id<double>())
			return EC_word(static_cast<double>(value_));
		else if (value_.variant().type() == boost::typeindex::type_id<int>())
			return EC_word(static_cast<int>(value_));
		else if (value_.variant().type() == boost::typeindex::type_id<long>())
			return EC_word(static_cast<long>(value_));
		else
			throw std::runtime_error("Unknown Constant type");
	}
	else if (
		value_.type().is<SymbolType>()
		|| value_.type().is<StringType>()
	)
		return EC_atom(value_.str().c_str());
	else if (value_.type().is<CompoundType>()) {
		EC_word field_list = ::nil();
		for (auto &pair : static_cast<const CompoundType::Representation &>(value_))
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
	else
		throw std::runtime_error("Unknown Constant type");
}


} // namespace gologpp
