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

#include "component.h"
#include <parser/platform/reference.h>

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_permutation.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/phoenix/fusion/at.hpp>

namespace gologpp {
namespace parser {


ComponentParser::ComponentParser()
: ComponentParser::base_type(component, "component_definition")
{
	component = (
		(lit("component") > r_name() > '{') [
			_val = new_<platform::Component>(new_<Scope>(_r1), _1)
		] > (
			( "states:" > state_list(*_val) )
			^ ( "clocks:" > clock_list(*_val) )
			^ ( "transitions:" > transition_list(*_val) )
		) > '}'
	) [
		phoenix::bind(&Scope::register_global, _r1, _val)
	];
	component.name("component_definition");
	on_error<rethrow>(component, delete_(_val));


	clock_list = r_name() [
		phoenix::bind(&platform::Component::add_clock, _r1, new_<platform::Clock>(_1, _r1))
	] % ',';
	clock_list.name("clocks_definition");


	state_list = (r_name() > -('(' > clock_formula(_r1) > ')')) [
		phoenix::bind(
			&platform::Component::add_state, _r1,
			new_<platform::State>(_1, _r1, _2)
		)
	] % ',';
	state_list.name("state_definition");


	transition = (
		platform_ref<platform::State>()(phoenix::bind(&platform::Component::m_scope, _r1))
		> exog_transition_arrow /* true if exog */
		> -('(' > clock_formula(_r1) > ')')
		> platform_ref<platform::State>()(phoenix::bind(&platform::Component::m_scope, _r1))
		> -(lit("resets") > '('
			> platform_ref<platform::Clock>()(phoenix::bind(&platform::Component::m_scope, _r1)) % ','
		> ')')
	) [
		if_(_2 /*is exog*/) [
			phoenix::bind(
				&platform::Component::add_exog_transition, _r1,
				new_<platform::ExogTransition>(_1, _4, _3, _5)
			)
		].else_[
			phoenix::bind(
				&platform::Component::add_transition, _r1,
				new_<platform::Transition>(_1, _4, _3, _5)
			)
		]
	];
	transition.name("transition");

	transition_list = transition(_r1) % ',';
	transition_list.name("transition_list");

	exog_transition_arrow = lit("->") [ _val = true ]
		| lit("=>") [ _val = false ];
}


} // namespace parser
} // namespace gologpp
