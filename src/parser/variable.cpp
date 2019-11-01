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

#include <string>

#include "variable.h"
#include "types.h"

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_sequence.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>

#include <model/scope.h>
#include <model/reference.h>
#include <model/variable.h>


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl() {
	static rule<shared_ptr<Variable>(Scope &)> rv {
		(any_type_specifier()(_r1) >> r_name()) [
			_val = phoenix::bind(
				&Scope::get_var, _r1,
				VarDefinitionMode::FORCE,
				_1, _2
			)
		],
		"variable_declaration"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage() {
	static rule<shared_ptr<Variable>(Scope &, Typename)> rv {
		r_name() [
			_val = phoenix::bind(
				&Scope::get_var, _r1,
				VarDefinitionMode::DENY,
				_r2, _1
			),
			_pass = !!_val // force conversion to bool
		],
		"typed_variable_reference"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Reference<Variable> *(Scope &, Typename)> &var_ref() {
	static rule<Reference<Variable> *(Scope &, Typename)> rv {
		var_usage()(_r1, _r2) [
			_val = new_<Reference<Variable>>(_1)
		]
	};

	return rv;
}


rule<shared_ptr<Variable>(Scope &)> &any_var_usage() {
	static rule<shared_ptr<Variable>(Scope &)> rv {
		r_name() [
			_val = phoenix::bind(
				&Scope::lookup_var, _r1,
				_1
			),
			_pass = !!_val // force conversion to bool
		],
		"variable_reference"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}






}
}
