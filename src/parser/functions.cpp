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

#include "mixed_member_access.h"
#include "functions.h"
#include "types.h"
#include "expressions.h"
#include "variable.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/optional/optional_io.hpp>

#include <model/scope.h>
#include <model/procedural.h>


namespace gologpp {
namespace parser {



FunctionParser::FunctionParser()
: FunctionParser::base_type(function, "function_definition")
{
	function =
		(
			(type_identifier<Type>()(_r1) >> "function")
			> r_name() > '('
		) [
			_a = new_<Scope>(_r1),
			_b = _2,
			_d = _1
		]
		> ( -(var_decl()(*_a) % ',') > ')' ) [
			_c = _1
		]
		> (
			lit(';') [
				_val = phoenix::bind(
					&Scope::declare_global<Function>,
					_r1, _a, *_d, _b, _c
				),
				_pass = !!_val
			]
			| (lit('=') > typed_expression()(*_a, *_d)) [
				_val = phoenix::bind(
					&Scope::define_global<Function, Expression *>,
					_r1, _a, *_d, _b, _c, _1
				),
				_pass = !!_val
			]
		)
	;
	function.name("function_definition");
	on_error<rethrow>(function, delete_(_a));
	GOLOGPP_DEBUG_NODE(function)
}



ProcedureParser::ProcedureParser()
: ProcedureParser::base_type(procedure, "procedure_definition")
{
	procedure =
		(lit("procedure") > r_name() > '(') [
			_a = new_<Scope>(_r1),
			_b = _1
		]
		> ( -(var_decl()(*_a) % ',') > ')' ) [
			_c = _1
		]
		> (
			lit(';') [
				_val = phoenix::bind(
					&Scope::declare_global<Procedure>,
					_r1, _a, void_type(), _b, _c
				),
				_pass = !!_val
			]
			| statement(*_a) [
				_val = phoenix::bind(
					&Scope::define_global<Procedure, Instruction *>,
					_r1, _a, void_type(), _b, _c, _1
				),
				_pass = !!_val
			]
		)
	;
	procedure.name("function_definition");
	on_error<rethrow>(procedure, delete_(_a));
	GOLOGPP_DEBUG_NODE(procedure)
}



} // namespace parser
} // namespace gologpp

