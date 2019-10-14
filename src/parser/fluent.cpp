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


FluentParser::FluentParser()
: FluentParser::base_type(fluent)
{
	fluent = (
		(((any_type_specifier()(_r1) >> "fluent") > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _2, // fluent name
			_d = _1  // type name
		] )
		> ( -(var_decl()(*_a) % ',') > lit(')')) [
			_c = _1
		]
	)
	> (
		lit(';') [ // forward declaration
			_val = phoenix::bind(
				&Scope::declare_global<Fluent>,
				_r1,
				_a, _d, _b, _c
			),
			_pass = !!_val
		]
		| ( lit('{') > ( // definition
			("initially:" > +initially(*_a, _d))
			^ ("domain:" > +domain_assignment()(*_a, false))
		) > '}' ) [
			_val = phoenix::bind(
				&Scope::define_global<
					Fluent,
					const boost::optional<vector<InitialValue *>> &
				>,
				_r1,
				_a, _d, _b, _c, _1
			),
			_pass = !!_val
		]
	);
	fluent.name("fluent_definition");
	on_error<rethrow>(fluent, delete_(_a));

	initial_val_arg = var_ref()(_r1, UndefinedType::name()) | any_literal();
	initial_val_arg.name("initial_value_argument");


	initially = (lit('(')
		> -(
			initial_val_arg(_r1) % ','
		) > ')' > '='
		>> literal()(_r2, false)
		> ';'
	) [
		_val = new_<InitialValue>(_1, _2)
	];
	initially.name("initial_value_mapping");

	GOLOGPP_DEBUG_NODES((fluent)(initially))
}



} // namespace parser
} // namespace gologpp

