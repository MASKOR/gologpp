#ifndef FORMULA_H
#define FORMULA_H

#include <vector>
#include <memory>

#include "Execution.h"


namespace gologpp {

using namespace std;


class Value;


class Expression : public LanguageElement, public enable_shared_from_this<Expression> {
public:
	virtual ~Expression();

//	virtual unique_ptr<Value> value() const = 0;
};


class Atom : public Expression {
};


class Variable : public Atom {
};


class Value : public Atom {
public:
	virtual size_t hash() const;
};


class Negation : public Expression {
public:
	Negation(const shared_ptr<Expression> &expression);

protected:
	const shared_ptr<Expression> expression_;
};


enum ComparisonOperator {
	eq = 1, neq, ge, gt, le, lt
};


class Comparison : public Expression {
public:
	Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs);

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
	ConnectiveFormula(const shared_ptr<Expression> &lhs, const shared_ptr<Expression> &rhs);

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


class Quantification {
public:
	Quantification(
	        const shared_ptr<Variable> &variable,
	        const shared_ptr<Expression> &expression);

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
