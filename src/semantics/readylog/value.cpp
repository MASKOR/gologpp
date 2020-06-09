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

#include "value.h"
#include "execution.h"
#include <model/value.h>

namespace gologpp {

Semantics<Value>::Semantics(const Value &value, ReadylogContext &context)
: AbstractSemantics<Value>(value, context)
{}

EC_word Semantics<Value>::plterm()
{
	if (element().type().is<BoolType>()) {
		if (static_cast<bool>(element()))
			return EC_atom("true");
		else
			return EC_atom("fail");
	}
	else if (element().type().is<NumberType>()) {
		if (element().representation().type() == boost::typeindex::type_id<double>())
			return EC_word(static_cast<double>(element()));
		else if (element().representation().type() == boost::typeindex::type_id<int>())
			return EC_word(static_cast<int>(element()));
		else if (element().representation().type() == boost::typeindex::type_id<long>())
			return EC_word(static_cast<long>(element()));
		else if (element().representation().type() == boost::typeindex::type_id<unsigned int>())
			return EC_word(static_cast<long>(static_cast<unsigned int>(element())));
		else if (element().representation().type() == boost::typeindex::type_id<unsigned long>())
			return EC_word(static_cast<long long>(static_cast<unsigned long>(element())));
		else
			throw std::runtime_error("Unknown Constant type");
	}
	else if (
		element().type().is<SymbolType>()
		|| element().type().is<StringType>()
	)
		return EC_atom(static_cast<string>(element()).c_str());
	else if (element().type().is<CompoundType>()) {
		EC_word field_list = ::nil();
		for (auto &pair : static_cast<const CompoundType::Representation &>(element()))
			field_list = ::list(
				::term(EC_functor(("#" + pair.first).c_str(), 1),
					pair.second->semantics().plterm()
				),
				field_list
			);
		return ::term(EC_functor("gpp_compound", 2),
			EC_atom(("#" + element().type().name()).c_str()),
			field_list
		);
	}
	else if (element().type().is<ListType>()) {
		EC_word list = ::nil();

		const ListType::Representation &list_repr = static_cast<const ListType::Representation &>(element());
		ListType::Representation::const_reverse_iterator it = list_repr.rbegin();
		while (it != list_repr.rend())
			list = ::list((*it++)->semantics().plterm(), list);
		return ::term(EC_functor("gpp_list", 2),
			EC_atom(("#" + dynamic_cast<const ListType &>(element().type()).element_type().name()).c_str()),
			list
		);
	}
	else if (element().type().is<UndefinedType>()) {
		return EC_atom("fail");
	}
	else
		throw std::runtime_error("Unknown Constant type");
}


Value pl_term_to_value(EC_word term) {
	EC_word type, list, list_head, list_tail;
	EC_atom did;
	EC_functor ftor;
	double d;
	long i;
	long long li;
	char *s;

	if (EC_succeed == term.is_long(&i))
		return Value(get_type<NumberType>(), i);
	else if (EC_succeed == term.is_long_long(&li)) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
// Compiler only looks at current architecture, but sizeof(long) needn't be equal to sizeof(long long)
// on all architectures
		if (li >= 0
			&& static_cast<unsigned long long>(li) <= static_cast<unsigned long long>(
				std::numeric_limits<unsigned long>::max()
			)
		)
			return Value(get_type<NumberType>(), static_cast<unsigned long>(li));
		else if (li >= std::numeric_limits<long>::min() && li <= std::numeric_limits<long>::max())
			return Value(get_type<NumberType>(), static_cast<long>(li));
		else
			throw std::runtime_error(std::to_string(li) + " exceeds value limits representable in golog++");
#pragma clang diagnostic pop
	}
	else if (EC_succeed == term.is_double(&d))
		return Value(get_type<NumberType>(), d);
	else if (EC_succeed == term.is_atom(&did)) {
		if (did == EC_atom("true"))
			return Value(get_type<BoolType>(), true);
		else if (did == EC_atom("fail"))
			return Value(get_type<BoolType>(), false);
		else
			return Value(get_type<SymbolType>(), string(did.name()));
	}
	else if (EC_succeed == term.is_string(&s))
		return Value(get_type<StringType>(), string(s));
	else if (
		term.functor(&ftor) == EC_succeed
		&& ftor.name() == string("gpp_list")
		&& ftor.arity() == 2
		&& term.arg(1, type) == EC_succeed
		&& type.is_atom(&did) == EC_succeed
		&& term.arg(2, list) == EC_succeed
		&& list.is_list(list_head, list_tail) == EC_succeed
	) {
		vector<Value *> list_repr;
		do {
			list_repr.push_back(new Value(pl_term_to_value(list_head)));
		} while (EC_succeed == list_tail.is_list(list_head, list_tail));

		string elem_t(did.name());
		elem_t = elem_t.substr(1); // remove # prefix that is added to avoid name clashes
		shared_ptr<const Type> elem_t_p = global_scope().lookup_type(elem_t);
		if (!elem_t_p)
			throw Bug("Undefined type: " + elem_t);
		shared_ptr<const ListType> list_t = global_scope().lookup_list_type(*elem_t_p);
		if (!list_t)
			throw Bug("Undefined type: list[" + elem_t + "]");

		return Value {
			*global_scope().lookup_list_type(*list_t),
			boost::optional<vector<Value *>> { list_repr }
		};
	}
	else if (
		term.functor(&ftor) == EC_succeed
		&& ftor.name() == string("gpp_compound")
		&& ftor.arity() == 2
		&& term.arg(1, type) == EC_succeed
		&& type.is_atom(&did) == EC_succeed
		&& term.arg(2, list) == EC_succeed
		&& list.is_list(list_head, list_tail) == EC_succeed
	) {
		vector<fusion_wtf_vector<string, Value *>> compound_repr;
		do {
			EC_functor field_ftor;
			EC_word field_value;
			if (
				list_head.functor(&field_ftor) == EC_succeed
				&& field_ftor.arity() == 1
				&& list_head.arg(0, field_value)
			) {
				compound_repr.push_back(
					fusion_wtf_vector<string, Value *> {
						string(field_ftor.name()).substr(1),
						new Value(pl_term_to_value(list_head))
					}
				);
			}
			else
				throw Bug("Invalid list entry: " + ReadylogContext::instance().to_string(list_head));
		} while (EC_succeed == list_tail.is_list(list_head, list_tail));

		string type_name(did.name());
		type_name = type_name.substr(1); // remove # prefix that is added to avoid name clashes
		shared_ptr<const CompoundType> type = global_scope().lookup_type<CompoundType>(type_name);
		if (!type)
			throw Bug("Undefined compound type: " + type_name);

		return Value(*type, compound_repr);
	}

	else
		throw Bug("Invalid prolog value: " + ReadylogContext::instance().to_string(term));
}

Semantics<Value> *Semantics<Value>::copy(const Value &target_elem) const
{ return new Semantics<Value>(target_elem, rl_context()); }



} // namespace gologpp
