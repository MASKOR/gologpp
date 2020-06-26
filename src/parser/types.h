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

#ifndef GOLOGPP_PARSER_TYPES_H_
#define GOLOGPP_PARSER_TYPES_H_

#include <model/gologpp.h>
#include <model/types.h>
#include <model/scope.h>

#include <boost/phoenix/function/adapt_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/operator/self.hpp>

#include "utilities.h"


namespace gologpp {
namespace parser {



struct TypeDefinitionParser : public grammar<void(Scope &)> {
	TypeDefinitionParser();

	rule<void(Scope &)> type_definition;
	rule<void(Scope &), locals<CompoundType *>> compound_type_def;
	rule<void(Scope &), locals<ListType *>> list_type_def;
};


template<class BaseT>
struct TypeNameParser : public grammar<shared_ptr<const Type>(Scope &)> {
	TypeNameParser();
	void init();

	rule<shared_ptr<const Type>(Scope &)> list_type_name;
	rule<shared_ptr<const Type>(Scope &)> type_name;
	rule<shared_ptr<const Type>(Scope &)> non_list_type_name;
};


template<class BaseT>
rule<shared_ptr<const BaseT>(Scope &)> &type_identifier() {
	static TypeNameParser<BaseT> tnp;
	static rule<shared_ptr<const BaseT>(Scope &)> rv {
		tnp(_r1) [
			_val = phoenix::bind(&std::dynamic_pointer_cast<const BaseT, const Type>, _1),
			_pass = !!_val
		]
	};
	return rv;
}

extern rule<shared_ptr<const Type>(Scope &)> any_type_name;



BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const UndefinedType &, undefined_type, gologpp::type<UndefinedType>)
BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const VoidType &, void_type, gologpp::type<VoidType>)
BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const BoolType &, bool_type, gologpp::type<BoolType>)
BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const NumberType &, number_type, gologpp::type<NumberType>)
BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const StringType &, string_type, gologpp::type<StringType>)
BOOST_PHOENIX_ADAPT_FUNCTION_NULLARY(const SymbolType &, symbol_type, gologpp::type<SymbolType>)


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_TYPES_H_
