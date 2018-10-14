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

#include <model/scope.h>
#include <model/procedural.h>


namespace gologpp {
namespace parser {

template<class ExpressionT>
FunctionParser<ExpressionT>::FunctionParser()
	: FunctionParser::base_type(function, type_descr<ExpressionT>() + "_function_definition")
	{
		function = function_forward(_r1) | function_def(_r1);
		function.name(type_descr<ExpressionT>() + "_function_definition");

		function_forward = (
			((type_mark<ExpressionT>() >> "function") >> r_name() >> '(') [
				_a = new_<Scope>(_r1)
			]
			>> -(abstract_var()(*_a) % ',') >> ')' >> ';'
		) [
			_val = phoenix::bind(
				&Scope::declare_global<Function<ExpressionT>>,
				phoenix::bind(&global_scope),
				_a, _1, _2
			)
		];
		function_forward.name(type_descr<ExpressionT>() + "_function_forward_declaration");

		function_def = (
			((type_mark<ExpressionT>() >> "function") >> r_name() >> '(') [
				_a = new_<Scope>(phoenix::bind(&AbstractLanguageElement::m_scope, _r1))
			]
			>> -(abstract_var()(*_a) % ',') >> ')' >> statement(_r1)
		) [
			_val = phoenix::bind(
				&Scope::define_global<Function<ExpressionT>, Statement *>,
				phoenix::bind(&global_scope),
				_a, _1, _2, _3
			)
		];
		function_def.name(type_descr<ExpressionT>() + "_function_definition");


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

