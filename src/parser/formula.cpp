#include <model/fluent.h>
#include <model/procedural.h>

#include "formula.h"
#include "types.h"
#include "field_access.h"
#include "expressions.h"
#include "atoms.h"
#include "list_access.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_plus.hpp>
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
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/core/nothing.hpp>

#include <unordered_map>
#include <functional>
#include <list>


namespace gologpp {
namespace parser {


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


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



using op_list = std::list<fusion_wtf_vector<Expression *, BooleanOperator>>;
/*
 * Behold the braindeadness of PEGs: Operator precedence is impossible to implement
 * because left recursion is forbidden. Insert facepalm here.
 *
 * So first, we have to parse sequences of unary expressions and operators into an unstructured
 * list. Then shove it all into this "after-parser" to build a structured BooleanOperation
 * that properly reflects operator precedence.
 */
BooleanOperation *parse_op_seq_(op_list vec, Expression *last)
{
	if (vec.size() == 1)
		return new BooleanOperation(
			at_c<0>(vec.front()),
			at_c<1>(vec.front()),
			last
		);
	else {
		BooleanOperator op = at_c<1>(vec.front());
		BooleanOperation *rhs = parse_op_seq_(op_list(++vec.begin(), vec.end()), last);
		if (rhs->op() > op)
			return new BooleanOperation(
				at_c<0>(vec.front()),
				op,
				rhs
			);
		else {
			// TODO: This is a memory leak, but what the heck this parser is full of them, anyways
			// and it's not the worst one, either.
			BooleanOperator op = at_c<1>(vec.back());
			BooleanOperation *lhs = parse_op_seq_(op_list(vec.begin(), --vec.end()), at_c<0>(vec.back()));
			return new BooleanOperation(
				lhs,
				op,
				last
			);
		}
	}
}



BooleanOperation *parse_op_seq(
	vector<fusion_wtf_vector<Expression *, BooleanOperator>> vec,
	Expression *rhs
) {
	return parse_op_seq_(
		op_list(vec.begin(), vec.end()),
		rhs
	);
}



BooleanExpressionParser::BooleanExpressionParser()
: BooleanExpressionParser::base_type(expression, "boolean_expression")
{
	expression = operation_seq(_r1) | unary_expr(_r1);
	expression.name("boolean_expression");

	unary_expr = quantification(_r1) | negation(_r1) | boolean_value()
		| bool_var_ref(_r1) | brace(_r1)
		| comparison(_r1)
		| typed_reference<Fluent>()(_r1, BoolType::name())
		| typed_reference<Function>()(_r1, BoolType::name())
		| field_access()(_r1, val(BoolType::name()))
		| list_access()(_r1, BoolType::name())
	;
	unary_expr.name("unary_boolean_expression");

	operation_seq = (
		+(unary_expr(_r1) >> bool_op) >> unary_expr(_r1)
	) [
		_val = phoenix::bind(&parse_op_seq, _1, _2)
	];
	operation_seq.name("operation_seq");


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

	bool_var_ref = var_usage()(_r1, val(BoolType::name())) [
		_val = new_<Reference<Variable>>(_1)
	];
	bool_var_ref.name("reference_to_boolean_variable");

	GOLOGPP_DEBUG_NODES(
		(expression)(unary_expr)
		(operation_seq)
		(bool_op)
		//(negation)(brace)(bool_var_ref)(quantification)
		//(quantification_op)
	)
}



} // namespace parser
} // namespace gologpp

