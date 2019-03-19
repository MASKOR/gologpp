#include "field_access.h"
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



namespace gologpp {
namespace parser {

template<class ActionT>
string action_keyword();

template<>
string action_keyword<Action>()
{ return "action"; }

template<>
string action_keyword<ExogAction>()
{ return "exog_action"; }



ActionDefinitionParser<Action>::ActionDefinitionParser()
: ActionDefinitionParser<Action>::base_type(definition, "action_definition")
{
	definition = ( lit('{') > (
		( "precondition:" > formula_parser(*_r2) )
		^ ( "effect:" > +(effect(*_r2) > ';') )
		^ ( "senses:" > senses(*_r2) )
		^ ( "domain:" > +(domain_assignment(*_r2)) )
		^ qi::eps
	) > '}' ) [
		phoenix::bind(
			&Scope::define_global<
				Action,
				boost::optional<BooleanExpression *>,
				boost::optional<vector<AbstractEffectAxiom *>>,
				boost::optional<Reference<AbstractFluent> *>
			>,
			_r1,
			_r2, _r3, _r4, _1, _2, _3
		)
	];

	effect = boolean_effect(_r1) | numeric_effect(_r1) | symbolic_effect(_r1) | string_effect(_r1) | compound_effect(_r1);
	senses = boolean_senses(_r1) | numeric_senses(_r1) | symbolic_senses(_r1) | string_senses(_r1) | compound_senses(_r1);
}



ActionDefinitionParser<ExogAction>::ActionDefinitionParser()
: ActionDefinitionParser<ExogAction>::base_type(definition, "action_definition")
{
	definition = ( lit('{') > (
		( "precondition:" > formula_parser(*_r2) )
		^ ( "effect:" > +(effect(*_r2) > ';') )
		^ ( "domain:" > +(domain_assignment(*_r2)) )
		^ qi::eps
	) > '}' ) [
		phoenix::bind(
			&Scope::define_global<
				ExogAction,
				boost::optional<BooleanExpression *>,
				boost::optional<vector<AbstractEffectAxiom *>>
			>,
			_r1,
			_r2, _r3, _r4, _1, _2
		)
	];

	effect = boolean_effect(_r1) | numeric_effect(_r1) | symbolic_effect(_r1) | string_effect(_r1) | compound_effect(_r1);
}



template<class ActionT>
ActionParser<ActionT>::ActionParser()
: ActionParser<ActionT>::base_type(action, "action_declaration")
{
	action = (
		((lit(action_keyword<ActionT>()) > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		])
		> ( -(abstract_var<VarDefinitionMode::FORCE>()(*_a) % ',') > ')' ) [
			_c = _1
		]
	)
	> (
		action_definition(_r1, _a, _b, _c)
		| lit(';') [
			phoenix::bind(&Scope::declare_global<ActionT>, _r1, _a, _b, _c)
		]
	);
	action.name("action_declaration");

	//GOLOGPP_DEBUG_NODES((action));
}


template
struct ActionParser<Action>;

template
struct ActionParser<ExogAction>;



} // namespace parser
} // namespace gologpp

