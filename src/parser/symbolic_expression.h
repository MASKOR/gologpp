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

#ifndef GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
#define GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_

#include "utilities.h"

#include <model/expressions.h>

namespace gologpp {
namespace parser {


struct SymbolicExpressionParser : grammar<Expression *(Scope &)> {
	SymbolicExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Reference<Variable> *(Scope &)> var_ref;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_SYMBOLIC_EXPRESSION_H_
