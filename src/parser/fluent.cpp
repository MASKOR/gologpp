#include "fluent.h"
#include "types.h"
#include "domain.h"

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
#include <boost/spirit/include/qi_permutation.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/optional/optional_io.hpp>

#include <model/fluent.h>
#include <model/expressions.h>


namespace gologpp {
namespace parser {


template<class ExprT>
FluentParser<ExprT>::FluentParser()
: FluentParser::base_type(fluent)
, variable(abstract_var<true>())
{

	fluent = (
		(((type_mark<ExprT>() >> "fluent") > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		] )
		> ( -(variable(*_a) % ',') > lit(')')) [
			_c = _1
		]
		> type_specifier<ExprT>() [
			_d = _1
		]
	)
	> (
		lit(';') [ // forward declaration
			_val = phoenix::bind(
				&Scope::declare_global<Fluent<ExprT>>,
				_r1,
				_a, _b, _c
			),
			if_(!phoenix::bind(&Fluent<ExprT>::set_type_by_name, *_val, _d)) [
				_pass = false
			]
		]
		| ( lit('{') > (// definition
			("initially:" > +initially)
			^ ("domain:" > +domain_assignment(*_a))
		) > '}' ) [
			_val = phoenix::bind(
				&Scope::define_global<
					Fluent<ExprT>,
					const boost::optional<vector<InitialValue<ExprT> *>> &
				>,
				_r1,
				_a, _b, _c, _1
			),
			if_(!phoenix::bind(&Fluent<ExprT>::set_type_by_name, *_val, _d)) [
				_pass = false
			]
		]
	);
	fluent.name(type_descr<ExprT>()() + "_fluent_definition");
	on_error<rethrow>(fluent, delete_(_a));

	initially = (lit('(') > -(abstract_constant<true>() % ',') > ')' > '=' > constant > ';') [
		_val = new_<InitialValue<ExprT>>(_1, _2)
	];
	initially.name("initial_value_mapping");

	GOLOGPP_DEBUG_NODES((variable)(fluent)(initially));
}


FluentParser<NumericExpression> numeric_fluent;
FluentParser<BooleanExpression> boolean_fluent;
FluentParser<SymbolicExpression> symbolic_fluent;
FluentParser<StringExpression> string_fluent;
FluentParser<CompoundExpression> compound_fluent;


rule<AbstractFluent *(Scope &)> &abstract_fluent() {
	static rule<AbstractFluent *(Scope &)> any_fluent {
		numeric_fluent(_r1) | boolean_fluent(_r1)
		| symbolic_fluent(_r1) | string_fluent(_r1)
		| compound_fluent(_r1),
		"any_fluent"
	};
	return any_fluent;
}


} // namespace parser
} // namespace gologpp

