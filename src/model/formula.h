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
	eq = 1, neq, ge, gt, le, lt
};


class Comparison : public BooleanExpression, public LanguageElement<Comparison> {
public:
	Comparison(NumericExpression *lhs, ComparisonOperator op, NumericExpression *rhs, Scope &parent_scope);
	ComparisonOperator cmp_operator() const;
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


class ConnectiveFormula : public BooleanExpression {
public:
	ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope);
	const BooleanExpression &lhs() const;
	const BooleanExpression &rhs() const;

protected:
	unique_ptr<BooleanExpression> lhs_;
	unique_ptr<BooleanExpression> rhs_;
};


class Conjunction : public ConnectiveFormula {
public:
	using ConnectiveFormula::ConnectiveFormula;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)
};


class Disjunction : public ConnectiveFormula {
public:
	using ConnectiveFormula::ConnectiveFormula;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)
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
