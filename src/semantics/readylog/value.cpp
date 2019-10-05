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
		if (value_.representation().type() == boost::typeindex::type_id<double>())
			return EC_word(static_cast<double>(value_));
		else if (value_.representation().type() == boost::typeindex::type_id<int>())
			return EC_word(static_cast<int>(value_));
		else if (value_.representation().type() == boost::typeindex::type_id<long>()
			|| value_.representation().type() == boost::typeindex::type_id<unsigned int>())
			return EC_word(static_cast<long>(value_));
		else if (value_.representation().type() == boost::typeindex::type_id<unsigned long>())
			return EC_word(static_cast<long long>(static_cast<unsigned long>(value_)));
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
		return ::term(EC_functor("gpp_compound", 2),
			EC_atom(("#" + value_.type_name()).c_str()),
			field_list
		);
	}
	else if (value_.type().is<ListType>()) {
		EC_word list = ::nil();

		const ListType::Representation &list_repr = static_cast<const ListType::Representation &>(value_);
		ListType::Representation::const_reverse_iterator it = list_repr.rbegin();
		while (it != list_repr.rend())
			list = ::list((*it++)->semantics().plterm(), list);
		return ::term(EC_functor("gpp_list", 2),
			EC_atom(("#" + dynamic_cast<const ListType &>(value_.type()).element_type().name()).c_str()),
			list
		);
	}
	else if (value_.type().is<UndefinedType>()) {
		return EC_atom("fail");
	}
	else
		throw std::runtime_error("Unknown Constant type");
}


} // namespace gologpp
