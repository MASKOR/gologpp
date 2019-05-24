#include "field_access.h"
#include "functions.h"
#include "types.h"
#include "statements.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/optional/optional_io.hpp>

#include <model/scope.h>
#include <model/procedural.h>


namespace gologpp {
namespace parser {

static rule<Typename(Scope &)> decl_prefix {
	lit("procedure") [
		_val = val(VoidType::name())
	]
	| any_type_specifier() [ _val = _1 ] >> "function"
};


FunctionParser::FunctionParser()
: FunctionParser::base_type(function, "function_definition")
{
	function =
		(decl_prefix(_r1) > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _2,
			_d = _1
		]
		> ( -(var_decl()(*_a) % ',') > ')' ) [
			_c = _1
		]
		> (
			lit(';') [
				_val = phoenix::bind(
					&Scope::declare_global<Function>,
					_r1, _a, _d, _b, _c
				),
				_pass = !!_val
			]
			| statement(*_a) [
				_val = phoenix::bind(
					&Scope::define_global<Function, Expression *>,
					_r1, _a, _d, _b, _c, _1
				),
				_pass = !!_val
			]
		)
	;
	function.name("function_definition");
	on_error<rethrow>(function, delete_(_a));
	GOLOGPP_DEBUG_NODE(function);
}




} // namespace parser
} // namespace gologpp

