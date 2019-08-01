#include "types.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/scope.h>



namespace gologpp {
namespace parser {


TypeDefinitionParser::TypeDefinitionParser()
: TypeDefinitionParser::base_type(type_definition, "compound_type_definition")
{
	type_definition = compound_type_def(_r1) | list_type_def(_r1);

	compound_type_def = (lit("compound") > r_name() > '{') [
		_val = new_<CompoundType>(_1)
	] > ((any_type_specifier() > r_name()) [
		phoenix::bind(&CompoundType::add_field, _val, _2, _1)
	] % ',' > lit('}')) [
		phoenix::bind(&Scope::register_type, _r1, _val)
	]
	;
	type_definition.name("compound_type_definition");
	on_error<rethrow>(type_definition, delete_(_val));

	list_type_def = (lit("list") > any_type_specifier() > "[]") [
		_val = new_<ListType>(_1),
		phoenix::bind(&Scope::register_type, _r1, _val)
	];
	list_type_def.name("list_type_definition");
}



rule<string()> &any_type_specifier() {
	static rule<string()> any_type_specifier_ = r_name() [
		if_(phoenix::bind(&Scope::exists_type, phoenix::bind(&global_scope), _1)) [
			_val = _1
		].else_ [
			_pass = false
		]
	];
	any_type_specifier_.name("any_type_specifier");

	return any_type_specifier_;
}


} // namespace parser
} // namespace gologpp
