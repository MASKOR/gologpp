#include "action.h"

#include <boost/phoenix/fusion/at.hpp>

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/fluent.h>


namespace gologpp {
namespace parser {

ActionParser::ActionParser()
: ActionParser::base_type(action, "action_declaration")
{
	action = action_forward(_r1) | action_def(_r1);

	action_forward = ( (
		(("action" >> r_name() >> '(') [
			_a = new_<Scope>(_r1)
		])
		>> -(abstract_var()(*_a) % ',') >> ')'
	) >> lit(';') ) [
		phoenix::bind(&Scope::declare_global<Action>, _r1, _a, _1, _2)
	];
	action_forward.name("action_forward_declaration");
	on_error<rethrow>(action_forward, delete_(_a));

	action_def = (
		(("action" >> r_name() >> '(') [
			_a = new_<Scope>(_r1)
		])
		>> -(abstract_var()(*_a) % ',') >> ')'
		>> ( '{'
			> -( "precondition:" > formula_parser(*_a) )
			> -( "effect:" > +(effect(*_a) > ';') )
			//> "signal:" //*/
		> '}' )
	) [
		phoenix::bind(
			&Scope::define_global<
				Action,
				boost::optional<BooleanExpression *>,
				boost::optional<vector<AbstractEffectAxiom *>>
			>,
			_r1,
			_a, _1, _2, at_c<0>(_3), at_c<1>(_3)
		)
	];
	action_def.name("action_definition");
	on_error<rethrow>(action_def, delete_(_a));

	effect = boolean_effect(_r1) | numeric_effect(_r1);

	BOOST_SPIRIT_DEBUG_NODES((action)(action_forward)(action_def));
}

} // namespace parser
} // namespace gologpp

