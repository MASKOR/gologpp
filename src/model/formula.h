#ifndef FORMULA_H
#define FORMULA_H

#include "expressions.h"
#include "scope.h"
#include "variable.h"

#include <vector>
#include <memory>


namespace gologpp {


class Negation : public BooleanExpression, public NoScopeOwner, public LanguageElement<Negation> {
public:
	Negation(BooleanExpression *expression);
	Negation(Negation &&) = default;
	virtual ~Negation() override = default;

	const BooleanExpression &expression() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	EQ = 1, NEQ, GE, GT, LE, LT
};


string to_string(ComparisonOperator op);


template<class ExprT>
class Comparison : public BooleanExpression, public NoScopeOwner, public LanguageElement<Comparison<ExprT>> {
public:
	Comparison(ExprT *lhs, ComparisonOperator op, ExprT *rhs)
	: lhs_(lhs)
	, op_(op)
	, rhs_(rhs)
	{
		lhs_->set_parent(this);
		rhs_->set_parent(this);
	}

	ComparisonOperator op() const
	{ return op_; }

	const ExprT &lhs() const
	{ return *lhs_; }

	const ExprT &rhs() const
	{ return *rhs_; }

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override
	{ return '(' + lhs().to_string(pfx) + ' ' + gologpp::to_string(op()) + ' ' + rhs().to_string(pfx) + ')'; }

protected:
	unique_ptr<ExprT> lhs_;
	ComparisonOperator op_;
	unique_ptr<ExprT> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	AND = 1, OR, XOR, IMPLIES, IFF
};

string to_string(BooleanOperator op);


class BooleanOperation : public BooleanExpression, public NoScopeOwner, public LanguageElement<BooleanOperation> {
public:
	BooleanOperation(
		BooleanExpression *lhs,
		BooleanOperator op,
		BooleanExpression *rhs
	);
	BooleanOperator op() const;
	const BooleanExpression &lhs() const;
	const BooleanExpression &rhs() const;

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


class AbstractVariable;

enum QuantificationOperator {
	EXISTS = 1, FORALL
};

string to_string(QuantificationOperator op);


class Quantification : public BooleanExpression, public ScopeOwner, public LanguageElement<Quantification> {
public:
	Quantification(
		Scope *own_scope,
		QuantificationOperator op,
		const shared_ptr<AbstractVariable> &variable,
		BooleanExpression *expression
	);

	QuantificationOperator op() const;
	const AbstractVariable &variable() const;
	const BooleanExpression &expression() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	QuantificationOperator op_;
	shared_ptr<AbstractVariable> variable_;
	unique_ptr<BooleanExpression> expression_;
};



} // namespace gologpp



#endif // FORMULA_H
