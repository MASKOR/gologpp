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

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/phoenix/fusion/at.hpp>

#include <model/scope.h>



namespace gologpp {
namespace parser {


TypeDefinitionParser::TypeDefinitionParser()
: TypeDefinitionParser::base_type(type_definition, "type_definition")
{
	type_definition = compound_type_def(_r1) | list_type_def(_r1);
	type_definition.name("type_definition");

	compound_type_def = (lit("compound") > r_name() > '{') [
		_val = new_<CompoundType>(_1)
	] > ((any_type_specifier()(_r1) > r_name()) [
		phoenix::bind(&CompoundType::add_field, _val, _2, _1)
	] % ',' > lit('}')) [
		phoenix::bind(&Scope::register_type, _r1, _val)
	];
	compound_type_def.name("compound_type_definition");
	on_error<rethrow>(type_definition, delete_(_val));

	list_type_def = (lit("list") >> '[' >> any_type_specifier()(_r1) >> ']') [
		_val = new_<ListType>(_1),
		phoenix::bind(&Scope::register_type, _r1, _val)
	];
	list_type_def.name("list_type_definition");

	GOLOGPP_DEBUG_NODES((type_definition)(compound_type_def)(list_type_def))
}



TypeNameParser::TypeNameParser()
: TypeNameParser::base_type(type_name, "type_specifier")
{
	type_name = (list_type_name(_r1) | r_name()) [
		if_(phoenix::bind(&Scope::exists_type, _r1, _1)) [
			_val = _1
		].else_[
			_pass = false
		]
	];
	type_name.name("type_name");

	list_type_name = (qi::string("list") >> qi::string("[") >> type_name(_r1) >> qi::string("]")) [
		_val = _1 + _2 + _3 + _4
	];
	list_type_name.name("list_type_name");

	//GOLOGPP_DEBUG_NODES((type_name)(list_type_name))
}


rule<Typename(Scope &)> &any_type_specifier() {
	static TypeNameParser tnp;
	static rule<Typename(Scope &)> rv { tnp(_r1) };
	rv.name("any_type_specifier");

	return rv;
}


} // namespace parser
} // namespace gologpp
