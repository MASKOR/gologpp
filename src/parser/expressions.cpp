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
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char.hpp>

// This ends up including deprecated /usr/include/boost/spirit/include/phoenix.hpp,
// So we're silencing the warning until boost is fixed.
#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/object/new.hpp>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_argument.hpp>

#include "types.h"
#include "domain.h"
#include "arithmetic.h"
#include "formula.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"
#include "list_expression.h"

#include <model/procedural.h>

#include <unordered_map>
#include <functional>

namespace gologpp {
namespace parser {


rule<Expression *(Scope &)> boolean_expression;
rule<Expression *(Scope &)> numeric_expression;
rule<Expression *(Scope &)> string_expression;
rule<Expression *(Scope &)> symbolic_expression;


rule<Conditional<Expression> *(Scope &, const Type &)> conditional_expression;


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

	conditional_expression = {
		(
			lit("if") > '(' > boolean_expression_(_r1) > ')'
			> typed_expression()(_r1, _r2)
			> "else"
			> typed_expression()(_r1, _r2)
		) [
			_val = new_<Conditional<Expression>>(_1, _2, _3)
		]
	};

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
		| compound_expression(_r1, undefined_type())
		| list_expression(_r1, undefined_type())
		, "value_expression"
	};
	//GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



rule<Expression *(Scope *, const Type *)> &get_expression_parser(const Type &t)
{
	static BooleanExpressionParser boolean_expression_;
	static NumericExpressionParser numeric_expression_;
	static StringExpressionParser string_expression_;
	static SymbolicExpressionParser symbolic_expression_;

	static rule<Expression *(Scope *, const Type *)> compound_expression_ {
		compound_expression(*_r1, *_r2)
		, "compound_expression"
	};

	static rule<Expression *(Scope *, const Type *)> list_expression_ {
		list_expression(*_r1, *_r2)
		, "list_expression"
	};

	static std::unordered_map <
		string,
		rule<Expression *(Scope *, const Type *)>
	> expr_parser_map {
		{ NumberType::static_name(), { numeric_expression_(*_r1), "numeric_expression" } },
		{ BoolType::static_name(), { boolean_expression_(*_r1), "boolean_expression" } },
		{ StringType::static_name(), { string_expression_(*_r1), "string_expression" } },
		{ SymbolType::static_name(), { symbolic_expression_(*_r1), "symbolic_expression" } },
	};

	//GOLOGPP_DEBUG_NODE(expr_parser_map[NumberType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[BoolType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[StringType::name()])
	//GOLOGPP_DEBUG_NODE(expr_parser_map[SymbolType::name()])

	auto it = expr_parser_map.find(t.name());
	if (it != expr_parser_map.end())
		return it->second;
	else {
		if (t.is<CompoundType>())
			return compound_expression_;
		else if (t.is<ListType>())
			return list_expression_;
		else if (t.is<Domain>())
			return get_expression_parser(
				dynamic_cast<const Domain &>(t).element_type()
			);
		else
			throw Bug("Unkown type " + t.name());
	}
}


rule<Expression *(Scope &, const Type &)> &typed_expression()
{
	/// This crazy hack allows us to pass arguments to lazy parsers.
	/// cf. @a https://stackoverflow.com/questions/56221006/boostspirit-lazy-parser-with-arguments
	/// Note that the argument to the lazily returned parser must be copyable, so the
	/// Scope& has to become Scope* here.
	static rule <
		Expression *(
			Scope &,
			const Type &,
			rule<Expression *(Scope *, const Type *)>
		)
	> lazyinvoke {
		lazy(phoenix::bind(_r3, &_r1, &_r2))
	};

	static rule<Expression *(Scope &, const Type &)> rv {
		lazyinvoke(_r1, _r2, phoenix::bind(&get_expression_parser, _r2))
		, "typed_expression"
	};
	//GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



}
}
