#ifndef FORMULA_H
#define FORMULA_H

#include "expressions.h"
#include "atoms.h"

#include <vector>
#include <memory>


namespace gologpp {


class Negation : public BooleanExpression, public LanguageElement<Negation> {
public:
	Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope);
	Negation(Negation &&) = default;
	virtual ~Negation() override = default;

	const BooleanExpression &expression() const;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

protected:
	unique_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	EQ = 1, NEQ, GE, GT, LE, LT
};


class Comparison : public BooleanExpression, public LanguageElement<Comparison> {
public:
	Comparison(NumericExpression *lhs, ComparisonOperator op, NumericExpression *rhs, Scope &parent_scope);
	ComparisonOperator op() const;
	const NumericExpression &lhs() const;
	const NumericExpression &rhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

protected:
	unique_ptr<NumericExpression> lhs_;
	ComparisonOperator op_;
	unique_ptr<NumericExpression> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	AND = 1, OR, XOR, IMPLIES, IFF
};


class BooleanOperation : public BooleanExpression {
public:
	BooleanOperation(BooleanExpression *lhs, BooleanOperator op, BooleanExpression *rhs, Scope &parent_scope);
	BooleanOperator op() const;
	const BooleanExpression &lhs() const;
	const BooleanExpression &rhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)
protected:
	unique_ptr<BooleanExpression> lhs_;
	BooleanOperator op_;
	unique_ptr<BooleanExpression> rhs_;
};



/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


class AbstractVariable;


class Quantification : public BooleanExpression {
public:
	Quantification(
	        const shared_ptr<AbstractVariable> &variable,
	        unique_ptr<BooleanExpression> &&expression,
	        Scope &parent_scope);

	const AbstractVariable &variable() const;
	const BooleanExpression &expression() const;

protected:
	shared_ptr<AbstractVariable> variable_;
	unique_ptr<BooleanExpression> expression_;
};


class ExistentialQuantification : public Quantification, public LanguageElement<ExistentialQuantification> {
public:
	using Quantification::Quantification;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *expression_)
};


class UniversalQuantification : public Quantification, public LanguageElement<UniversalQuantification> {
public:
	using Quantification::Quantification;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *expression_)
};


} // namespace gologpp



#endif // FORMULA_H
