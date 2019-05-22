#ifndef FORMULA_H
#define FORMULA_H

#include "expressions.h"
#include "scope.h"
#include "variable.h"

#include <vector>
#include <memory>


namespace gologpp {


class Negation : public Expression, public NoScopeOwner, public LanguageElement<Negation> {
public:
	Negation(Expression *expression);
	virtual ~Negation() override = default;

	const Expression &expression() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	EQ = 1, NEQ, GE, GT, LE, LT
};


string to_string(ComparisonOperator op);


class Comparison : public Expression, public NoScopeOwner, public LanguageElement<Comparison> {
public:
	Comparison(Expression *lhs, ComparisonOperator op, Expression *rhs)
	: lhs_(lhs)
	, op_(op)
	, rhs_(rhs)
	{
		ensure_type_equality(*lhs, *rhs);
		lhs_->set_parent(this);
		rhs_->set_parent(this);
	}

	ComparisonOperator op() const
	{ return op_; }

	const Expression &lhs() const
	{ return *lhs_; }

	const Expression &rhs() const
	{ return *rhs_; }

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override
	{ return '(' + lhs().to_string(pfx) + ' ' + gologpp::to_string(op()) + ' ' + rhs().to_string(pfx) + ')'; }

protected:
	unique_ptr<Expression> lhs_;
	ComparisonOperator op_;
	unique_ptr<Expression> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	AND = 1, OR, XOR, IMPLIES, IFF
};

string to_string(BooleanOperator op);


class BooleanOperation : public Expression, public NoScopeOwner, public LanguageElement<BooleanOperation> {
public:
	BooleanOperation(
		Expression *lhs,
		BooleanOperator op,
		Expression *rhs
	);
	BooleanOperator op() const;
	const Expression &lhs() const;
	const Expression &rhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> lhs_;
	BooleanOperator op_;
	unique_ptr<BooleanExpression> rhs_;
};



/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


enum QuantificationOperator {
	EXISTS = 1, FORALL
};

string to_string(QuantificationOperator op);


class Quantification : public Expression, public ScopeOwner, public LanguageElement<Quantification> {
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

	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	QuantificationOperator op_;
	shared_ptr<Variable> variable_;
	unique_ptr<BooleanExpression> expression_;
};



} // namespace gologpp



#endif // FORMULA_H
