#ifndef FORMULA_H
#define FORMULA_H

#include "atoms.h"

#include <vector>
#include <memory>


namespace gologpp {

using namespace std;


class Negation : public Expression {
public:
	Negation(const shared_ptr<Expression> &expression, InScope &parent_expr);

protected:
	const shared_ptr<Expression> expression_;
};


enum ComparisonOperator {
	eq = 1, neq, ge, gt, le, lt
};


class Comparison : public Expression {
public:
	Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, InScope &parent_expr);

protected:
	const shared_ptr<Atom> lhs_;
	const ComparisonOperator op_;
	const shared_ptr<Atom> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

class ConnectiveFormula : public Expression {
public:
	ConnectiveFormula(const shared_ptr<Expression> &lhs, const shared_ptr<Expression> &rhs, InScope &parent_expr);

protected:
	const shared_ptr<Expression> lhs_, rhs_;
};


class Conjunction : public ConnectiveFormula {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


class Disjunction : public ConnectiveFormula {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


class Implication : public ConnectiveFormula {
public:
	using ConnectiveFormula::ConnectiveFormula;
};


/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


class Quantification : public Expression {
public:
	Quantification(
	        const shared_ptr<Variable> &variable,
	        const shared_ptr<Expression> &expression,
	        InScope &parent_expr);

protected:
	const shared_ptr<Variable> variable_;
	const shared_ptr<Expression> expression_;
};


class ExistentialQuantification : public Quantification {
public:
	using Quantification::Quantification;
};


class UniversalQuantification : public Quantification {
public:
	using Quantification::Quantification;
};


} // namespace gologpp



#endif // FORMULA_H
