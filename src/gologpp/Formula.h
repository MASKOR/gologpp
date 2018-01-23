#ifndef FORMULA_H
#define FORMULA_H

#include "atoms.h"

#include <vector>
#include <memory>


namespace gologpp {
namespace generic {

using namespace std;


class BooleanExpression : public Expression {
public:
	using Expression::Expression;
};


class Negation : public BooleanExpression, public LanguageElement<Negation> {
public:
	Negation(const shared_ptr<BooleanExpression> &expression, const shared_ptr<Scope> &parent_scope);

protected:
	const shared_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	eq = 1, neq, ge, gt, le, lt
};


class Comparison : public BooleanExpression, public LanguageElement<Comparison> {
public:
	Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, const shared_ptr<Scope> &parent_scope);

protected:
	const shared_ptr<Atom> lhs_;
	const ComparisonOperator op_;
	const shared_ptr<Atom> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

class ConnectiveFormula : public BooleanExpression {
public:
	ConnectiveFormula(const shared_ptr<BooleanExpression> &lhs, const shared_ptr<BooleanExpression> &rhs, const shared_ptr<Scope> &parent_scope);

protected:
	const shared_ptr<BooleanExpression> lhs_, rhs_;
};


class Conjunction : public ConnectiveFormula, public LanguageElement<Conjunction> {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


class Disjunction : public ConnectiveFormula, public LanguageElement<Disjunction> {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


class Implication : public ConnectiveFormula, public LanguageElement<Implication> {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


class Quantification : public BooleanExpression {
public:
	Quantification(
	        const shared_ptr<Variable> &variable,
	        const shared_ptr<BooleanExpression> &expression,
	        const shared_ptr<Scope> &parent_scope);

protected:
	const shared_ptr<Variable> variable_;
	const shared_ptr<BooleanExpression> expression_;
};


class ExistentialQuantification : public Quantification, public LanguageElement<ExistentialQuantification> {
public:
	using Quantification::Quantification;
};


class UniversalQuantification : public Quantification, public LanguageElement<UniversalQuantification> {
public:
	using Quantification::Quantification;
};


} // namespace generic
} // namespace gologpp



#endif // FORMULA_H
