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

#pragma once

#include <parser/utilities.h>
#include <parser/platform/reference.h>

#include <model/platform/constraint.h>

namespace gologpp {
namespace parser {


namespace helper {

/* Intermediate, unified representation for binary temporal and boolean
 * constraint operations. Required for compatibility with the precedence parser */
class BinaryOpIntermediate : public Expression {
public:
	class Operator {
	public:
		enum class OpType {
			AND, OR, UNTIL, SINCE
		};

		Operator() = default;

		Operator(
			OpType type,
			boost::optional < fusion_wtf_vector <
				boost::optional<Value *>,
				boost::optional<Value *>
			> > bound
		);
		~Operator();

		OpType type() const;
		Value *lower_bound();
		Value *upper_bound();

	private:
		OpType type_;
		Value *lower_bound_ = nullptr, *upper_bound_ = nullptr;
	};

	BinaryOpIntermediate(Expression *lhs, Operator op, Expression *rhs);

	Operator op() const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Scope &scope() override;
	virtual const Scope &scope() const override;

	Expression *convert();

private:
	Expression *lhs_, *rhs_;
	Operator op_;
};

unsigned int precedence(const BinaryOpIntermediate::Operator &op);

} // namespace helper



template<class RefT>
struct ConstraintSpecParser : public grammar<Expression *(Scope &)> {
	ConstraintSpecParser();
	void init();

	rule<Expression *(Scope &)> constraint_spec;
	rule<helper::BinaryOpIntermediate *(Scope &)> binary_sequence;
	rule<Expression *(Scope &)> braced_expr;
	rule<platform::TemporalUnaryOperation *(Scope &)> temporal_unary;
	rule<platform::TemporalUnaryOperation::Operator()> temporal_unary_op;
	rule<helper::BinaryOpIntermediate::Operator()> binary_op;
	rule<helper::BinaryOpIntermediate::Operator()> temporal_binary_op;
	rule <
		fusion_wtf_vector <
			boost::optional<Value *>,
			boost::optional<Value *>
		> ()
	> bound;

	rule<Expression *(Scope &)> unary_expr;

	rule<DurativeCall::Hook()> action_hook_name;
	rule<platform::ActionHook *(Scope &)> action_hook;
	rule<platform::During *(Scope &)> during;
	PlatformRefParser<Action> action_ref;

	rule <
		platform::StateAssertion *(Scope &),
		locals<platform::Reference<platform::Component> *>
	> state_assertion;
};



struct ConstraintSectionParser : public grammar<void(Scope &)> {
	ConstraintSectionParser();

	rule<void(Scope &)> constraint_section;
	rule<platform::Constraint *(Scope &)> constraint;
	ConstraintSpecParser<Action> action_spec;
	ConstraintSpecParser<platform::State> state_spec;
};




} // namespace parser
} // namespace gologpp
