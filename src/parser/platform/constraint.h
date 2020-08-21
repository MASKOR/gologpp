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
template<class SubjectT>
class BinaryOpIntermediate : public SubjectT {
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

	BinaryOpIntermediate(SubjectT *lhs, Operator op, SubjectT *rhs);
	BinaryOpIntermediate(Expression *lhs, Operator op, Expression *rhs);

	Operator op() const;

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Scope &scope() override;
	virtual const Scope &scope() const override;

	SubjectT *convert();

private:
	SubjectT *lhs_, *rhs_;
	Operator op_;
};

unsigned int precedence(const typename BinaryOpIntermediate<platform::StateSpec>::Operator &op);
unsigned int precedence(const typename BinaryOpIntermediate<platform::ActionSpec>::Operator &op);

} // namespace helper



template<class SubjectT>
struct ConstraintSpecParser : public grammar<SubjectT *(Scope &)> {
	ConstraintSpecParser();
	void init();

	// (Nonterminal) Symbols which are common to both lhs and rhs of a constraint
	rule<SubjectT *(Scope &)> constraint_spec;
	rule<helper::BinaryOpIntermediate<SubjectT> *(Scope &)> binary_sequence;
	rule<SubjectT *(Scope &)> braced_expr;
	rule<platform::TemporalUnaryOperation<SubjectT> *(Scope &)> temporal_unary;
	rule<typename platform::TemporalUnaryOperation<SubjectT>::Operator()> temporal_unary_op;
	rule<typename helper::BinaryOpIntermediate<SubjectT>::Operator()> binary_op;
	rule<typename helper::BinaryOpIntermediate<SubjectT>::Operator()> temporal_binary_op;
	rule <
		fusion_wtf_vector <
			boost::optional<Value *>,
			boost::optional<Value *>
		> ()
	> bound;

	// Symbols which are specific to lhs/rhs.
	// Defined in the respective template specializations.
	rule<SubjectT *(Scope &)> unary_expr;
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
	ConstraintSpecParser<platform::ActionSpec> action_spec;
	ConstraintSpecParser<platform::StateSpec> state_spec;
};




} // namespace parser
} // namespace gologpp
