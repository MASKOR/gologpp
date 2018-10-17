#include "fluent.h"
#include "types.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/fluent.h>


namespace gologpp {
namespace parser {


FluentParser<BooleanExpression> boolean_fluent;
FluentParser<NumericExpression> numeric_fluent;
FluentParser<SymbolicExpression> symbolic_fluent;


template<class ExprT>
FluentParser<ExprT>::FluentParser()
: FluentParser::base_type(fluent)
, fluent_name(r_name())
, variable(abstract_var<true>())
{
	fluent = fluent_forward(_r1) | fluent_def(_r1);
	fluent.name(type_descr<ExprT>() + "_fluent_declaration");

	fluent_forward = (
		(((type_mark<ExprT>() >> "fluent") >> fluent_name >> '(') [
			_a = new_<Scope>(_r1)
		] )
		>> (variable(*_a) % ',') >> lit(')') >> ';'
	) [ // forward declaration
		_val = phoenix::bind(
			&Scope::declare_global<Fluent<ExprT>>,
			_r1,
			_a, _1, _2
		)
	];
	fluent_forward.name(type_descr<ExprT>() + "fluent_forward_declaration");
	on_error<rethrow>(fluent_forward, delete_(_a));

	fluent_def = (
		(((type_mark<ExprT>() >> "fluent") >> fluent_name >> '(') [
			_a = new_<Scope>(_r1)
		] )
		>> (variable(*_a) % ',') >> ')'
		>> ( lit('{') // definition
			> "initially:"
			> +initially
		> '}' )
	) [
		_val = phoenix::bind(
			&Scope::define_global<
				Fluent<ExprT>,
				const vector<InitialValue<ExprT> *> &
			>,
			_r1,
			_a, _1, _2, _3
		)
	];
	fluent_def.name(type_descr<ExprT>() + "fluent_definition");
	on_error<rethrow>(fluent_def, delete_(_a));

	initially = (lit('(') > -(abstract_constant<true>() % ',') > ')' > '=' > constant<ExprT, true>() > ';') [
		_val = new_<InitialValue<ExprT>>(_1, _2)
	];
	initially.name("initial_value_mapping");

	BOOST_SPIRIT_DEBUG_NODES((fluent_name)(variable)(fluent)(fluent_forward)(fluent_def)(initially));
}


} // namespace parser
} // namespace gologpp

