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

#ifndef GOLOGPP_PARSER_ARITHMETIC_H_
#define GOLOGPP_PARSER_ARITHMETIC_H_

#include "utilities.h"

#include <model/arithmetic.h>


namespace gologpp {
namespace parser {

/*********************
* Numeric expressions
*********************/

struct NumericExpressionParser : grammar<Expression *(Scope &)> {
	NumericExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> binary_expr;
	rule<Expression *(Scope &)> operation;
	rule<Expression *(Scope &)> brace;
	rule<Expression *(Scope &)> unary_expr;
	rule<Expression *(Scope &)> list_length;
	rule<ArithmeticOperation::Operator()> arith_operator;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ARITHMETIC_H_

