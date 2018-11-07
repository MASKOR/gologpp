#include "functions.h"
#include "types.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/delete.hpp>

#include <boost/optional/optional_io.hpp>

#include <model/scope.h>
#include <model/procedural.h>


namespace gologpp {
namespace parser {

template<class ExpressionT>
FunctionParser<ExpressionT>::FunctionParser()
: FunctionParser::base_type(function, type_descr<ExpressionT>() + "_function_definition")
{
	function =
		((type_mark<ExpressionT>() >> "function") > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		]
		> ( -(abstract_var<true>()(*_a) % ',') > ')' ) [
			_c = _1
		]
		> (
			lit(';') [
				_val = phoenix::bind(
					&Scope::declare_global<Function<ExpressionT>>,
					_r1, _a, _b, _c
				)
			]
			| statement(_r1) [
				_val = phoenix::bind(
					&Scope::define_global<Function<ExpressionT>, VoidExpression *>,
					_r1, _a, _b, _c, _1
				)
			]
		)
	;
	function.name(type_descr<ExpressionT>() + "_function_definition");
	on_error<rethrow>(function, delete_(_a));
	BOOST_SPIRIT_DEBUG_NODE(function);
}


AbstractFunctionParser::AbstractFunctionParser()
: AbstractFunctionParser::base_type(function, "any_function_definition")
{
	function = bool_func(_r1) | num_func(_r1) | sym_func(_r1);
	function.name("any_function_definition");
	BOOST_SPIRIT_DEBUG_NODE(function);
}


} // namespace parser
} // namespace gologpp

