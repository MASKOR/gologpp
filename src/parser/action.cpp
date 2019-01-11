#include "action.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_permutation.hpp>

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
	action = (
		(("action" > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		])
		> ( -(abstract_var<true>()(*_a) % ',') > ')' ) [
			_c = _1
		]
	)
	> (
		lit(';') [
			phoenix::bind(&Scope::declare_global<Action>, _r1, _a, _b, _c)
		]
		| ( lit('{') > (
			( "precondition:" > formula_parser(*_a) )
			^ ( "effect:" > +(effect(*_a) > ';') )
			^ ( "domain:" > +(domain_assignment(*_a)) )
			//> "signal:" //*/
		) > '}' ) [
			phoenix::bind(
				&Scope::define_global<
					Action,
					boost::optional<BooleanExpression *>,
					boost::optional<vector<AbstractEffectAxiom *>>
				>,
				_r1,
				_a, _b, _c, _1, _2
			)
		]
	);
	action.name("action_declaration");

	effect = boolean_effect(_r1) | numeric_effect(_r1) | symbolic_effect(_r1);

	//BOOST_SPIRIT_DEBUG_NODES((action));
}

} // namespace parser
} // namespace gologpp

