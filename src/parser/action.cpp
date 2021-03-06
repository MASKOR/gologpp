/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "action.h"
#include "variable.h"
#include "value.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_permutation.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_real.hpp>

#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/action.h>
#include <model/effect_axiom.h>

#include <boost/optional/optional_io.hpp>

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



template<>
ActionDefinitionParser<Action>::ActionDefinitionParser()
: ActionDefinitionParser<Action>::base_type(definition, "action_definition")
{
	static qi::real_parser<double> real_num;
	definition = ( lit('{') > (
		( "precondition:" > boolean_expression(*_r2) )
		^ ( "effect:" > +(effect(*_r2) > ';') )
		^ ( "senses:" > senses(*_r2) > ';' )
		^ ( "mapping:" > mapping(*_r2) )
		^ ( "silent:" > boolean_value() )
		^ ( "duration:" > duration )
		^ qi::eps
	) > '}' ) [
		_a = phoenix::bind(
			&Scope::define_global<
				Action,
				boost::optional<Expression *>,
				boost::optional<vector<AbstractEffectAxiom *>>,
				boost::optional<AbstractAssignment *>,
				boost::optional<BackendMapping *>
			>,
			_r1,
			_r2, undefined_type(), _r3, _r4, _1, _2, _3, _4
		),
		phoenix::bind(&AbstractAction::set_silent_v, _a, _5),
		phoenix::bind(&Action::set_duration, _a, _6)
		// Have to use two binds because the maximum number of arguments is limited :facepalm:
	];

	senses = senses_fluent(_r1) | senses_list_element(_r1) | senses_compound_field(_r1);
	senses.name("sensing_assignment");

	duration = (lit('[') > real_num > ',' > real_num > ']') [
		_val = phoenix::construct<Clock::DurationRange>(_1, _2)
	];

	effect = list_effect(_r1) | field_effect(_r1) | fluent_effect(_r1);
	effect.name("effect_axiom");
	GOLOGPP_DEBUG_NODES(
		(definition)(effect)
	)
}



template<>
ActionDefinitionParser<ExogAction>::ActionDefinitionParser()
: ActionDefinitionParser<ExogAction>::base_type(definition, "action_definition")
{
	definition = ( lit('{') > (
		( "precondition:" > boolean_expression(*_r2) )
		^ ( "effect:" > +(effect(*_r2) > ';') )
		^ ( "mapping:" > mapping(*_r2) )
		^ ( "silent:" > boolean_value() )
		^ qi::eps
	) > '}' ) [
		_a = phoenix::bind(
			&Scope::define_global<
				ExogAction,
				boost::optional<Expression *>,
				boost::optional<vector<AbstractEffectAxiom *>>,
				boost::optional<BackendMapping *>
			>,
			_r1,
			_r2, undefined_type(), _r3, _r4, _1, _2, _3
		),
		phoenix::bind(&AbstractAction::set_silent_v, _a, _4)
		// Have to use two binds because the maximum number of arguments is limited :facepalm:
	];

	effect = list_effect(_r1) | field_effect(_r1) | fluent_effect(_r1);
	effect.name("effect_axiom");
	GOLOGPP_DEBUG_NODES(
		(definition)(effect)
	)
}



template<class ActionT>
ActionParser<ActionT>::ActionParser()
: ActionParser<ActionT>::base_type(action, "action_declaration")
{
	action = (
		((lit(action_keyword<ActionT>()) > r_name() > '(') [
			_a = phoenix::bind(&Scope::get_scope, _r1, _1),
			_b = _1
		])
		> ( -(var_decl()(*_a) % ',') > ')' ) [
			_c = _1
		]
	)
	> (
		action_definition(_r1, _a, _b, _c)
		| lit(';') [
			phoenix::bind(&Scope::declare_global<ActionT>, _r1, _a, undefined_type(), _b, _c)
		]
	);
	action.name("action_declaration");

	GOLOGPP_DEBUG_NODES((action));
}


template
struct ActionParser<Action>;

template
struct ActionParser<ExogAction>;



} // namespace parser
} // namespace gologpp

