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

#ifndef GOLOGPP_PARSER_FORMULA_H_
#define GOLOGPP_PARSER_FORMULA_H_

#include <model/formula.h>

#include "utilities.h"


namespace gologpp {
namespace parser {


struct ComparisonParser : grammar<Comparison *(Scope &), locals<shared_ptr<const Type>>> {
	ComparisonParser();

	rule<Comparison *(Scope &), locals<shared_ptr<const Type>>> comparison;
	rule<ComparisonOperator()> cmp_op;
	rule<Expression *(Scope &)> comparable_expr;
};



struct BooleanExpressionParser : grammar<Expression *(Scope &)> {
	BooleanExpressionParser();

	rule<Expression *(Scope &)> expression;
	rule<Expression *(Scope &)> unary_expr;
	rule<Expression *(Scope &)> operation_seq;
	rule<Expression *(Scope &)> negation;
	rule<Expression *(Scope &)> brace;
	rule<Expression *(Scope &)> bool_var_ref;
	rule<Expression *(Scope &), locals<Scope *>> quantification;
	rule<QuantificationOperator()> quantification_op;
	rule<BooleanOperator()> bool_op;
	ComparisonParser comparison;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FORMULA_H_

