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

#include "expressions.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_lazy.hpp>
#include <boost/spirit/include/qi_plus.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/self.hpp>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_argument.hpp>

#include "arithmetic.h"
#include "formula.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"
#include "list_expression.h"
#include "list_access.h"
#include "field_access.h"

#include <unordered_map>
#include <functional>

namespace gologpp {
namespace parser {


rule<Expression *(Scope &)> boolean_expression;
rule<Expression *(Scope &)> numeric_expression;
rule<Expression *(Scope &)> string_expression;
rule<Expression *(Scope &)> symbolic_expression;


void initialize_cyclic_expressions()
{
	static BooleanExpressionParser boolean_expression_;
	static NumericExpressionParser numeric_expression_;
	static StringExpressionParser string_expression_;
	static SymbolicExpressionParser symbolic_expression_;

	boolean_expression = { boolean_expression_(_r1), "boolean_expression" };
	numeric_expression = { numeric_expression_(_r1), "numeric_expression" };
	string_expression = { string_expression_(_r1), "string_expression" };
	symbolic_expression = { symbolic_expression_(_r1), "symbolic_expression" };

	initialize_list_exprs();
	initialize_compound_exprs();

	GOLOGPP_DEBUG_NODES(
		//(boolean_expression)(numeric_expression)
		//(string_expression)(symbolic_expression)
	)
}


rule<Expression *(Scope &)> &value_expression()
{
	static rule<Expression *(Scope &)> rv {
		numeric_expression(_r1)
		| boolean_expression(_r1)
		| string_expression(_r1)
		| symbolic_expression(_r1)
		| compound_expression(_r1)
		| list_expression(_r1)
		, "value_expression"
	};
	//GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



static rule<Expression *(Scope *)> &get_expression_parser(Typename t)
{
	static BooleanExpressionParser boolean_expression_;
	static NumericExpressionParser numeric_expression_;
	static StringExpressionParser string_expression_;
	static SymbolicExpressionParser symbolic_expression_;

	static rule<Expression *(Scope *)> compound_expression_ {
		compound_expression(*_r1)
		, "compound_expression"
	};

	static rule<Expression *(Scope *)> list_expression_ {
		list_expression(*_r1)
		, "list_expression"
	};

	static std::unordered_map <
		Typename,
		rule<Expression *(Scope *)>
	> expr_parser_map {
		{ NumberType::name(), { numeric_expression_(*_r1), "numeric_expression" } },
		{ BoolType::name(), { boolean_expression_(*_r1), "boolean_expression" } },
		{ StringType::name(), { string_expression_(*_r1), "string_expression" } },
		{ SymbolType::name(), { symbolic_expression_(*_r1), "symbolic_expression" } },
	};

	//GOLOGPP_DEBUG_NODE(expr_parser_map[NumberType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[BoolType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[StringType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[SymbolType::name()])

	auto it = expr_parser_map.find(t);
	if (it != expr_parser_map.end())
		return it->second;
	else {
		shared_ptr<const Type> type = global_scope().lookup_type(t);
		if (type->is<CompoundType>())
			return compound_expression_;
		else if (type->is<ListType>())
			return list_expression_;
		else
			throw Bug("Unkown type " + t);
	}
}


rule<Expression *(Scope &, Typename)> &typed_expression()
{
	/// This crazy hack allows us to pass arguments to lazy parsers.
	/// cf. @a https://stackoverflow.com/questions/56221006/boostspirit-lazy-parser-with-arguments
	/// Note that the argument to the lazily returned parser must be copyable, so the
	/// Scope& has to become Scope* here.
	static rule <
		Expression *(
			Scope &,
			rule<Expression *(Scope *)>
		)
	> lazyinvoke {
		lazy(phoenix::bind(_r2, &_r1))
	};

	static rule<Expression *(Scope &, Typename)> rv {
		lazyinvoke(_r1, phoenix::bind(&get_expression_parser, _r2))
		, "typed_expression"
	};
	//GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



}
}
