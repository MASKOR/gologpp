#ifndef FORMULA_H
#define FORMULA_H

#include "atoms.h"

#include <vector>
#include <memory>


namespace gologpp {


class BooleanExpression : public Expression {
public:
	BooleanExpression(BooleanExpression &&);
	using Expression::Expression;
};


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
	Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, Scope &parent_scope);
	//DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

protected:
	shared_ptr<Atom> lhs_;
	ComparisonOperator op_;
	shared_ptr<Atom> rhs_;
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


class Quantification : public BooleanExpression {
public:
	Quantification(
	        const shared_ptr<Variable> &variable,
	        unique_ptr<BooleanExpression> &&expression,
	        Scope &parent_scope);

	const Variable &variable() const;
	const BooleanExpression &expression() const;

protected:
	shared_ptr<Variable> variable_;
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
