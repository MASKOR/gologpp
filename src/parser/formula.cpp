#include <model/fluent.h>
#include <model/procedural.h>

#include "formula.h"
#include "types.h"
#include "field_access.h"
#include "expressions.h"
#include "atoms.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <unordered_map>
#include <functional>


namespace gologpp {
namespace parser {



ComparisonParser::ComparisonParser()
: ComparisonParser::base_type(comparison, "comparison")
{
	cmp_op =
		qi::string(">") [ _val = val(ComparisonOperator::GT) ]
		| qi::string(">=") [ _val = val(ComparisonOperator::GE) ]
		| qi::string("<=") [ _val = val(ComparisonOperator::LE) ]
		| qi::string("<") [ _val = val(ComparisonOperator::LT) ]
		| qi::string("==") [ _val = val(ComparisonOperator::EQ) ]
		| qi::string("!=") [ _val = val(ComparisonOperator::NEQ) ]
	;
	cmp_op.name("comparison_operator");

	comparison = (
		(comparable_expr(_r1) >> cmp_op) [
			_a = phoenix::bind(&Expression::type_name, *_1)
		]
		> typed_expr(_r1, _a)
	) [
		_val = new_<Comparison>(at_c<0>(_1), at_c<1>(_1), _2)
	];
	comparison.name("comparison");

	typed_expr = comparable_expr(_r1) [
		_val = _1,
		_pass = phoenix::bind(&Expression::type_name, *_val) == _r2
	];

	comparable_expr = numeric_expression(_r1)
		| string_expression(_r1)
		| symbolic_expression(_r1);
	comparable_expr.name("comparable_lhs");

	GOLOGPP_DEBUG_NODES((comparison)(comparable_expr)(typed_expr))
}



BooleanExpressionParser::BooleanExpressionParser()
: BooleanExpressionParser::base_type(expression, "boolean_expression")
{
	expression = binary_expr(_r1) | unary_expr(_r1);
	expression.name("boolean_expression");

	unary_expr = quantification(_r1) | negation(_r1) | boolean_constant()
		| bool_var_ref(_r1) | brace(_r1)
		| comparison(_r1)
		| boolean_fluent_ref(_r1) | boolean_function_ref(_r1)
		| field_access()(_r1, val(Bool::static_name()))
	;
	unary_expr.name("unary_boolean_expression");

	binary_expr = (
		(unary_expr(_r1) >> bool_op) > expression(_r1)
	) [
		_val = new_<BooleanOperation>(at_c<0>(_1), at_c<1>(_1), _2)
	];
	binary_expr.name("binary_boolean_expression");

	quantification = ( (quantification_op > '(') [
		_a = new_<Scope>(_r1)
	] > var_decl()(*_a) > ')' > expression(*_a)) [
		_val = new_<Quantification>(_a, _1, _2, _3)
	];
	quantification.name("quantification");
	on_error<rethrow>(quantification, delete_(_a));

	quantification_op = qi::string("exists") [ _val = val(QuantificationOperator::EXISTS) ]
		| qi::string("forall") [ _val = val(QuantificationOperator::FORALL) ];
	quantification_op.name("quantification_operator");

	bool_op =
		qi::string("==") [ _val = val(BooleanOperator::IFF) ]
		| qi::string("!=") [ _val = val(BooleanOperator::XOR) ]
		| qi::string("&") [ _val = val(BooleanOperator::AND) ]
		| qi::string("|") [ _val = val(BooleanOperator::OR) ]
		| qi::string("->") [ _val = val(BooleanOperator::IMPLIES) ]
	;
	bool_op.name("boolean_operator");


	negation = '!' > unary_expr(_r1) [
		_val = new_<Negation>(_1)
	];
	negation.name("negation");

	brace = '(' >> expression(_r1) >> ')';
	brace.name("braced_boolean_expression");

	bool_var_ref = var_usage()(_r1, val(Bool::static_name())) [
		_val = new_<Reference<Variable>>(_1)
	];
	bool_var_ref.name("reference_to_boolean_variable");

	GOLOGPP_DEBUG_NODES((expression)(unary_expr)(binary_expr)
	(negation)(brace)(bool_var_ref)(quantification)
	(quantification_op)(bool_op))
}



} // namespace parser
} // namespace gologpp

