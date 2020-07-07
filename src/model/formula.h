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

#ifndef FORMULA_H
#define FORMULA_H

#include "expressions.h"
#include "scope.h"
#include "variable.h"

#include <vector>
#include <memory>


namespace gologpp {


class Negation : public Expression, public NoScopeOwner, public LanguageElement<Negation, BoolType> {
public:
	Negation(Expression *expression);
	virtual ~Negation() override = default;

	const Expression &expression() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> expression_;
};



class Comparison : public Expression, public NoScopeOwner, public LanguageElement<Comparison, BoolType> {
public:
	enum class Operator {
		EQ = 1, NEQ, GE, GT, LE, LT
	};

	Comparison(Expression *lhs, Operator op, Expression *rhs);

	Operator op() const;
	const Expression &lhs() const;
	const Expression &rhs() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

protected:
	unique_ptr<Expression> lhs_;
	Operator op_;
	unique_ptr<Expression> rhs_;
};

using ComparisonOperator = Comparison::Operator;

string to_string(Comparison::Operator op);




/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/


class BooleanOperation : public Expression, public NoScopeOwner, public LanguageElement<BooleanOperation, BoolType> {
public:
	enum class Operator {
		IMPLIES, OR, AND, XOR, IFF
	};

	BooleanOperation(
		Expression *lhs,
		Operator op,
		Expression *rhs
	);
	Operator op() const;
	const Expression &lhs() const;
	const Expression &rhs() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> lhs_;
	Operator op_;
	SafeExprOwner<BoolType> rhs_;
};

using BooleanOperator = BooleanOperation::Operator;

string to_string(BooleanOperator op);

unsigned int precedence(BooleanOperation::Operator op);



/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/

class Quantification : public Expression, public ScopeOwner, public LanguageElement<Quantification, BoolType> {
public:
	enum class Operator {
		EXISTS = 1, FORALL
	};

	Quantification(
		Scope *own_scope,
		Operator op,
		const shared_ptr<Variable> &variable,
		Expression *expression
	);

	Operator op() const;
	const Variable &variable() const;
	const Expression &expression() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*variable_, *expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	Operator op_;
	shared_ptr<Variable> variable_;
	SafeExprOwner<BoolType> expression_;
};

using QuantificationOperator = Quantification::Operator;

string to_string(QuantificationOperator op);




} // namespace gologpp



#endif // FORMULA_H
