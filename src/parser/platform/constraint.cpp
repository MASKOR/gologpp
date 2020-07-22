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

#include "constraint.h"
#include <parser/value.h>
#include <parser/reference.h>
#include <parser/platform/reference.h>

#include <model/platform/constraint.h>
#include <model/platform/component.h>
#include <model/value.h>

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_real.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/phoenix/fusion/at.hpp>

namespace gologpp {
namespace parser {


namespace helper {

BinaryOpIntermediate::Operator::Operator(
	BinaryOpIntermediate::Operator::OpType type,
	boost::optional<fusion_wtf_vector<
		boost::optional<Value *>,
		boost::optional<Value *>
	> > bound
)
: type_(type)
{
	double tmin = Clock::duration::min().count();
	double tmax = Clock::duration::max().count();
	if (bound) {
		if (at_c<0>(bound.get()))
			lower_bound_ = at_c<0>(bound.get()).get();
		else
			lower_bound_ = new Value(get_type<NumberType>(), tmin);

		if (at_c<1>(bound.get()))
			upper_bound_ = at_c<1>(bound.get()).get();
		else
			upper_bound_ = new Value(get_type<NumberType>(), tmax);
	}
	else {
		lower_bound_ = new Value(get_type<NumberType>(), tmin);
		upper_bound_ = new Value(get_type<NumberType>(), tmax);
	}
}

BinaryOpIntermediate::Operator::~Operator()
{
	delete lower_bound_;
	delete upper_bound_;
}

BinaryOpIntermediate::Operator::OpType BinaryOpIntermediate::Operator::type() const
{ return type_; }

Value *BinaryOpIntermediate::Operator::lower_bound()
{ return lower_bound_; }

Value *BinaryOpIntermediate::Operator::upper_bound()
{ return upper_bound_; }



BinaryOpIntermediate::BinaryOpIntermediate(Expression *lhs, BinaryOpIntermediate::Operator op, Expression *rhs)
: lhs_(lhs), rhs_(rhs), op_(op)
{}

BinaryOpIntermediate::Operator BinaryOpIntermediate::op() const
{ return op_; }

void BinaryOpIntermediate::attach_semantics(SemanticsFactory &)
{ throw Bug(string(__func__) + ": Not implemented"); }

string BinaryOpIntermediate::to_string(const string &) const
{ throw Bug(string(__func__) + ": Not implemented"); }

Scope &BinaryOpIntermediate::scope()
{ throw Bug(string(__func__) + ": Not implemented"); }

const Scope &BinaryOpIntermediate::scope() const
{ throw Bug(string(__func__) + ": Not implemented"); }

Expression *BinaryOpIntermediate::convert()
{
	BinaryOpIntermediate *lhs_inter = dynamic_cast<BinaryOpIntermediate *>(lhs_);
	BinaryOpIntermediate *rhs_inter = dynamic_cast<BinaryOpIntermediate *>(rhs_);

	Expression *lhs_final = nullptr, *rhs_final = nullptr;

	if (lhs_inter) {
		lhs_final = lhs_inter->convert();
		delete lhs_;
	}
	else
		lhs_final = lhs_;

	if (rhs_inter) {
		rhs_final = rhs_inter->convert();
		delete rhs_;
	}
	else
		rhs_final = rhs_;

	if (op().type() == Operator::OpType::OR)
		return new platform::BooleanConstraintOperation(
			lhs_final,
			platform::BooleanConstraintOperation::Operator::OR,
			rhs_final
		);
	else if (op().type() == Operator::OpType::AND)
		return new platform::BooleanConstraintOperation(
			lhs_final,
			platform::BooleanConstraintOperation::Operator::AND,
			rhs_final
		);
	else if (op().type() == Operator::OpType::SINCE)
		return new platform::TemporalBinaryOperation(
			lhs_final,
			rhs_final,
			platform::TemporalBinaryOperation::Operator::SINCE,
			op().lower_bound(),
			op().upper_bound()
		);
	else if (op().type() == Operator::OpType::UNTIL)
		return new platform::TemporalBinaryOperation(
			lhs_final,
			rhs_final,
			platform::TemporalBinaryOperation::Operator::UNTIL,
			op().lower_bound(),
			op().upper_bound()
		);

	throw Bug("Unhandled BinaryOpIntermediate::Operator::OpType");
}


unsigned int precedence(const BinaryOpIntermediate::Operator &op)
{
	switch (op.type()) {
	case BinaryOpIntermediate::Operator::OpType::UNTIL:
	case BinaryOpIntermediate::Operator::OpType::SINCE:
		return 1;
	case BinaryOpIntermediate::Operator::OpType::OR:
		return 2;
	case BinaryOpIntermediate::Operator::OpType::AND:
		return 3;
	}
	throw Bug("Unhandled BinaryOpIntermediate::Operator::OpType");
}

} // namespace helper



template<class RefT>
ConstraintSpecParser<RefT>::ConstraintSpecParser()
: ConstraintSpecParser::base_type(constraint_spec, debug_name<RefT>() + "_spec")
{
	constraint_spec = binary_sequence(_r1) [ _val = phoenix::bind(&helper::BinaryOpIntermediate::convert, _1) ]
		| unary_expr(_r1) [ _val = _1 ];
	constraint_spec.name(debug_name<RefT>() + "_spec");

	binary_sequence = (
		+(unary_expr(_r1) >> binary_op) >> unary_expr(_r1)
	) [
		_val = phoenix::bind(&parse_op_precedence<helper::BinaryOpIntermediate>, _1, _2)
	];
	binary_sequence.name("binary_op_seq");

	binary_op =
		lit('&') [
			_val = construct<helper::BinaryOpIntermediate::Operator>(
				helper::BinaryOpIntermediate::Operator::OpType::AND,
				boost::none
			)
		]
		| lit('|') [
			_val = construct<helper::BinaryOpIntermediate::Operator>(
				helper::BinaryOpIntermediate::Operator::OpType::OR,
				boost::none
			)
		]
		| (lit("until") > -bound) [
			_val = construct<helper::BinaryOpIntermediate::Operator>(
				helper::BinaryOpIntermediate::Operator::OpType::UNTIL,
				_1
			)
		]
		| (lit("since") > -bound) [
			_val = construct<helper::BinaryOpIntermediate::Operator>(
				helper::BinaryOpIntermediate::Operator::OpType::SINCE,
				_1
			)
		]
	;
	binary_op.name("binary_operator");

	braced_expr = (lit('(') > constraint_spec(_r1) > ')');
	braced_expr.name("braced_expr");

	temporal_unary = (
		temporal_unary_op > -bound > unary_expr(_r1)
	) [
		_val = new_<platform::TemporalUnaryOperation>(_3, _1, _2)
	];
	temporal_unary.name("temporal_unary_expr");

	temporal_unary_op =
		lit("next") [ _val = platform::TemporalUnaryOperation::Operator::NEXT ]
		| lit("previous") [ _val = platform::TemporalUnaryOperation::Operator::PREVIOUS ]
		| lit("future") [ _val = platform::TemporalUnaryOperation::Operator::FUTURE ]
		| lit("past") [ _val = platform::TemporalUnaryOperation::Operator::PAST ]
	;
	temporal_unary_op.name("temporal_unary_operator");

	bound = lit('[') > -numeric_value() > ',' > -numeric_value() > ']';
	bound.name("t_bound");

	init();
}


template<>
void ConstraintSpecParser<Action>::init()
{
	unary_expr = braced_expr(_r1)
		| temporal_unary(_r1)
		| action_hook(_r1)
		| during(_r1)
	;
	unary_expr.name("unary_action_expr");

	action_hook = (action_hook_name > '(' > action_ref(_r1, void_type()) > ')') [
		_val = new_<platform::ActionHook>(_1, _2)
	];

	action_hook_name =
		lit("start") [ _val = DurativeCall::Hook::START ]
		| lit("end") [ _val = DurativeCall::Hook::END ]
		| lit("cancel") [ _val = DurativeCall::Hook::CANCEL ]
		| lit("fail") [ _val = DurativeCall::Hook::FAIL ]
		| lit("finish") [ _val = DurativeCall::Hook::FINISH ]
	;
	action_hook_name.name("action_hook");

	during = (lit("during") > '(' > action_ref(_r1, void_type()) > ')') [
		_val = new_<platform::During>(_1)
	];
	during.name("during_action");
}


template<>
void ConstraintSpecParser<platform::State>::init()
{
	unary_expr = braced_expr(_r1)
		| temporal_unary(_r1)
		| state_assertion(_r1)
	;
	unary_expr.name("unary_state_expr");

	state_assertion = (
		lit("state") > '('
		> platform_ref<platform::Component>()(_r1) [
			_a = _1
		]
		> ')'
		> '=' > platform_ref<platform::State>()(phoenix::bind(&platform::Component::m_scope, **_a))
	) [
		_val = new_<platform::StateAssertion>(_1, _2)
	];
	state_assertion.name("state_assertion");
}


ConstraintSectionParser::ConstraintSectionParser()
: ConstraintSectionParser::base_type(constraint_section, "constraint_section")
{
	constraint_section = lit("constraints") > '{' > +(constraint(_r1) > ';') > '}';
	constraint_section.name("constraint_section");

	constraint = (action_spec(_r1) > ':' > state_spec(_r1)) [
		_val = new_<platform::Constraint>(_1, _2)
	];
	constraint.name("platform_constraint");
}


} // namespace parser
} // namespace gologpp
