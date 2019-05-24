#include "expressions.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_lazy.hpp>

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

#include <unordered_map>
#include <functional>

namespace gologpp {
namespace parser {


rule<Expression *(Scope &)> boolean_expression;
rule<Expression *(Scope &)> numeric_expression;
rule<Expression *(Scope &)> string_expression;
rule<Expression *(Scope &)> symbolic_expression;
rule<Expression *(Scope &)> compound_expression;


void define_expression_rules()
{
	static BooleanExpressionParser boolean_expression_;
	static NumericExpressionParser numeric_expression_;
	static StringExpressionParser string_expression_;
	static SymbolicExpressionParser symbolic_expression_;
	static CompoundExpressionParser compound_expression_;

	boolean_expression = { boolean_expression_(_r1), "boolean_expression" };
	numeric_expression = { numeric_expression_(_r1), "numeric_expression" };
	string_expression = { string_expression_(_r1), "string_expression" };
	symbolic_expression = { symbolic_expression_(_r1), "symbolic_expression" };
	compound_expression = { compound_expression_(_r1), "compound_expression" };

	GOLOGPP_DEBUG_NODES((boolean_expression)(numeric_expression)
		(string_expression)(symbolic_expression)(compound_expression))
}


rule<Expression *(Scope &)> &value_expression()
{
	static rule<Expression *(Scope &)> rv {
		numeric_expression(_r1)
		| boolean_expression(_r1)
		| string_expression(_r1)
		| symbolic_expression(_r1)
		| compound_expression(_r1)
		, "value_expression"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



static rule<Expression *(Scope *)> &get_expression_parser(Typename t)
{
	static BooleanExpressionParser boolean_expression_;
	static NumericExpressionParser numeric_expression_;
	static StringExpressionParser string_expression_;
	static SymbolicExpressionParser symbolic_expression_;
	static CompoundExpressionParser compound_expression_;

	static std::unordered_map <
		Typename,
		rule<Expression *(Scope *)>
	> expr_parser_map {
		{ NumberType::name(), { numeric_expression_(*_r1), "numeric_expression" } },
		{ BoolType::name(), { boolean_expression_(*_r1), "boolean_expression" } },
		{ StringType::name(), { string_expression_(*_r1), "string_expression" } },
		{ SymbolType::name(), { symbolic_expression_(*_r1), "symbolic_expression" } },
	};

	GOLOGPP_DEBUG_NODE(expr_parser_map[NumberType::name()])
	GOLOGPP_DEBUG_NODE(expr_parser_map[BoolType::name()])
	GOLOGPP_DEBUG_NODE(expr_parser_map[StringType::name()])
	GOLOGPP_DEBUG_NODE(expr_parser_map[SymbolType::name()])

	static rule<Expression *(Scope *)> compound_expression {
		compound_expression_(*_r1)
		, "compound_expression"
	};

	auto it = expr_parser_map.find(t);
	if (it == expr_parser_map.end())
		return compound_expression;
	else
		return it->second;
}


rule<Expression *(Scope &, Typename)> &typed_expression()
{
	/// This crazy hack allows us to pass arguments to lazy parsers.
	/// cf. @a https://stackoverflow.com/questions/56221006/boostspirit-lazy-parser-with-arguments
	/// Note that the argument to the lazily returned parser must be copyable, so the
	/// Scope& has to become Scope* here.
	static rule<
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
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


}
}
