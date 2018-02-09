#ifndef FORMULA_H
#define FORMULA_H

#include "atoms.h"

#include <vector>
#include <memory>


namespace gologpp {
namespace generic {


class BooleanExpression : public Expression {
protected:
	using Expression::Expression;
	BooleanExpression(BooleanExpression &&) = default;
};


class Negation : public BooleanExpression, public LanguageElement<Negation> {
public:
	Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope);
	Negation(Negation &&) = default;
	virtual ~Negation() override = default;

	const BooleanExpression &expression() const;

protected:
	unique_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	eq = 1, neq, ge, gt, le, lt
};


class Comparison : public BooleanExpression, public LanguageElement<Comparison> {
public:
	Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, Scope &parent_scope);

protected:
	shared_ptr<Atom> lhs_;
	ComparisonOperator op_;
	shared_ptr<Atom> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	conjunction = 1,
	disjunction,
	implication
};


class ConnectiveFormula : public BooleanExpression, public LanguageElement<ConnectiveFormula> {
public:
	ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, BooleanOperator op, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope);

protected:
	unique_ptr<BooleanExpression> lhs_;
	BooleanOperator op_;
	unique_ptr<BooleanExpression> rhs_;
};


/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


class Quantification : public BooleanExpression, public LanguageElement<Quantification> {
public:
	Quantification(
	        const shared_ptr<Variable> &variable,
	        unique_ptr<BooleanExpression> &&expression,
	        Scope &parent_scope);

protected:
	shared_ptr<Variable> variable_;
	unique_ptr<BooleanExpression> expression_;
};


class ExistentialQuantification : public Quantification {
public:
	using Quantification::Quantification;
};


class UniversalQuantification : public Quantification {
public:
	using Quantification::Quantification;
};


} // namespace generic
} // namespace gologpp



#endif // FORMULA_H
