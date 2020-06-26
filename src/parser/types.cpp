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

#include "types.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_as_string.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_attr_cast.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/phoenix/fusion/at.hpp>

#include <model/scope.h>



namespace gologpp {
namespace parser {


template<class BaseT>
void TypeNameParser<BaseT>::init()
{
	type_name = list_type_name(_r1) | non_list_type_name(_r1);
	type_name.name("type_name");

	list_type_name = (lit("list") > "[" > any_type_name(_r1) > "]") [
		_val = phoenix::bind(&Scope::lookup_list_type, _r1, *_1),
		_pass = !!_val
	];
	list_type_name.name("list_type_name");

	non_list_type_name = r_name() [
		_val = phoenix::bind(&Scope::lookup_type<BaseT>, _r1, _1),
		_pass = !!_val
	];
	non_list_type_name.name("non_list_type_name");

	//GOLOGPP_DEBUG_NODES((type_name)(list_type_name))
}


template<class BaseT>
TypeNameParser<BaseT>::TypeNameParser()
: TypeNameParser::base_type(non_list_type_name, BaseT::static_name() + "_type_name")
{ init(); }

template<>
TypeNameParser<Type>::TypeNameParser()
: TypeNameParser::base_type(type_name, "type_name")
{ init(); }

template<>
TypeNameParser<ListType>::TypeNameParser()
: TypeNameParser::base_type(list_type_name, "list_type_name")
{ init(); }

template<>
TypeNameParser<CompoundType>::TypeNameParser()
: TypeNameParser::base_type(non_list_type_name, "compound_type_name")
{ init(); }


rule<shared_ptr<const Type>(Scope &)> any_type_name {
	type_identifier<Type>()(_r1)
};



#define GOLOGPP_INSTANTIATE_TYPE_NAME_PARSER(_r, _data, T) \
	template \
	struct TypeNameParser<T>;

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_INSTANTIATE_TYPE_NAME_PARSER, (), GOLOGPP_PREDEFINED_TYPES)


TypeDefinitionParser::TypeDefinitionParser()
: TypeDefinitionParser::base_type(type_definition, "type_definition")
{
	type_definition = compound_type_def(_r1) | list_type_def(_r1);
	type_definition.name("type_definition");

	compound_type_def = (lit("compound") > r_name() > '{') [
		_a = new_<CompoundType>(_1)
	] > ((type_identifier<Type>()(_r1) > r_name()) [
		phoenix::bind(&CompoundType::add_field, _a, _2, *_1)
	] % ',' > lit('}')) [
		phoenix::bind(&Scope::register_type_raw, _r1, _a)
	];
	compound_type_def.name("compound_type_definition");
	on_error<rethrow>(compound_type_def, delete_(_a));

	list_type_def = (lit("list") >> '[' >> type_identifier<Type>()(_r1) >> ']') [
		_a = new_<ListType>(*_1),
		phoenix::bind(&Scope::register_type_raw, _r1, _a)
	];
	list_type_def.name("list_type_definition");

	GOLOGPP_DEBUG_NODES((type_definition)(compound_type_def)(list_type_def))
}




} // namespace parser
} // namespace gologpp
