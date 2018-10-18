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
#include <model/expressions.h>


namespace gologpp {
namespace parser {


template<class ExprT>
FluentParser<ExprT>::FluentParser()
: FluentParser::base_type(fluent)
, fluent_name(r_name())
, variable(abstract_var<true>())
{

	fluent = (
		(((type_mark<ExprT>() >> "fluent") > fluent_name > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		] )
		> ( -(variable(*_a) % ',') > lit(')')) [
			_c = _1
		]
	)
	> (
		lit(';') [ // forward declaration
			_val = phoenix::bind(
				&Scope::declare_global<Fluent<ExprT>>,
				_r1,
				_a, _b, _c
			)
		]
		| (lit('{') // definition
			> "initially:"
			> +initially
		> '}' ) [
			_val = phoenix::bind(
				&Scope::define_global<
					Fluent<ExprT>,
					const vector<InitialValue<ExprT> *> &
				>,
				_r1,
				_a, _b, _c, _1
			)
		]
	);
	fluent.name(type_descr<ExprT>() + "fluent_definition");
	on_error<rethrow>(fluent, delete_(_a));

	initially = (lit('(') > -(abstract_constant<true>() % ',') > ')' > '=' > constant<ExprT, true>() > ';') [
		_val = new_<InitialValue<ExprT>>(_1, _2)
	];
	initially.name("initial_value_mapping");

	BOOST_SPIRIT_DEBUG_NODES((fluent_name)(variable)(fluent)(initially));
}


FluentParser<NumericExpression> numeric_fluent;
FluentParser<BooleanExpression> boolean_fluent;
FluentParser<SymbolicExpression> symbolic_fluent;


rule<AbstractFluent *(Scope &)> &abstract_fluent() {
	static rule<AbstractFluent *(Scope &)> any_fluent {
		numeric_fluent(_r1) | boolean_fluent(_r1) | symbolic_fluent(_r1)
	};
	return any_fluent;
}


} // namespace parser
} // namespace gologpp

