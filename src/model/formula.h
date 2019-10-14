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


enum ComparisonOperator {
	EQ = 1, NEQ, GE, GT, LE, LT
};


string to_string(ComparisonOperator op);


class Comparison : public Expression, public NoScopeOwner, public LanguageElement<Comparison, BoolType> {
public:
	Comparison(Expression *lhs, ComparisonOperator op, Expression *rhs);

	ComparisonOperator op() const;
	const Expression &lhs() const;
	const Expression &rhs() const;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

protected:
	unique_ptr<Expression> lhs_;
	ComparisonOperator op_;
	unique_ptr<Expression> rhs_;
};




/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	// Order is important here since the
	// enum value encodes operator precedence
	IMPLIES = 0, OR, AND, XOR, IFF
	, MIN_PRECEDENCE = IMPLIES
	, MAX_PRECEDENCE = IFF
};

string to_string(BooleanOperator op);


class BooleanOperation : public Expression, public NoScopeOwner, public LanguageElement<BooleanOperation, BoolType> {
public:
	BooleanOperation(
		Expression *lhs,
		BooleanOperator op,
		Expression *rhs
	);
	BooleanOperator op() const;
	const Expression &lhs() const;
	const Expression &rhs() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> lhs_;
	BooleanOperator op_;
	SafeExprOwner<BoolType> rhs_;
};



/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


enum QuantificationOperator {
	EXISTS = 1, FORALL
};

string to_string(QuantificationOperator op);


class Quantification : public Expression, public ScopeOwner, public LanguageElement<Quantification, BoolType> {
public:
	Quantification(
		Scope *own_scope,
		QuantificationOperator op,
		const shared_ptr<Variable> &variable,
		Expression *expression
	);

	QuantificationOperator op() const;
	const Variable &variable() const;
	const Expression &expression() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*variable_, *expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	QuantificationOperator op_;
	shared_ptr<Variable> variable_;
	SafeExprOwner<BoolType> expression_;
};



} // namespace gologpp



#endif // FORMULA_H
