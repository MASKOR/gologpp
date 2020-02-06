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

#include "symbolic_expression.h"
#include "variable.h"
#include "value.h"
#include "mixed_member_access.h"
#include "reference.h"
#include "expressions.h"

#include <model/fluent.h>
#include <model/procedural.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {


SymbolicExpressionParser::SymbolicExpressionParser()
: SymbolicExpressionParser::base_type(expression, "symbolic_expression")
{
	expression = symbolic_value()
		| var_ref(_r1)
		| conditional_expression(_r1, symbol_type())
		| fluent_ref(_r1, symbol_type())
		| function_ref(_r1, symbol_type())
		| mixed_member_access()(_r1, symbol_type())
	;
	expression.name("symbolic_expression");

	var_ref = var_usage()(_r1, symbol_type()) [
		_val = new_<Reference<Variable>>(_1)
	];
	var_ref.name("reference_to_symbolic_variable");

	GOLOGPP_DEBUG_NODES((expression)(var_ref))
}


} // namespace parser
} // namespace gologpp
