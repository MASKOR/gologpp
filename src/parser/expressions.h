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

#ifndef GOLOGPP_PARSER_EXPRESSIONS_H_
#define GOLOGPP_PARSER_EXPRESSIONS_H_

#include <model/expressions.h>
#include "utilities.h"

namespace gologpp {
namespace parser {


/****************************
* General expressions:
* Just an undefined template
****************************/

rule<Expression *(Scope *, const Type *)> &get_expression_parser(const Type &t);

void initialize_cyclic_expressions();

rule<Expression *(Scope &)> &value_expression();

rule<Expression *(Scope &, const Type &)> &typed_expression();

extern rule<Conditional<Expression> *(Scope &, const Type &)> conditional_expression;


extern rule<Expression *(Scope &)> boolean_expression;
extern rule<Expression *(Scope &)> numeric_expression;
extern rule<Expression *(Scope &)> string_expression;
extern rule<Expression *(Scope &)> symbolic_expression;
extern rule<Expression *(Scope &, const Type &)> compound_expression;
extern rule<Expression *(Scope &, const Type &)> list_expression;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EXPRESSIONS_H_

