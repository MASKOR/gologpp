#include <cctype>

#include "Formula.h"


namespace gologpp {


Expression::~Expression()
{}



Negation::Negation(const shared_ptr<Expression> &expression)
: expression_(expression)
{}


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs)
    : lhs_(lhs), op_(op), rhs_(rhs)
{}


ConnectiveFormula::ConnectiveFormula(const shared_ptr<Expression> &lhs, const shared_ptr<Expression> &rhs)
: lhs_(lhs), rhs_(rhs)
{}


Quantification::Quantification(const shared_ptr<Variable> &variable, const shared_ptr<Expression> &expression)
: variable_(variable), expression_(expression)
{}


} // namespace gologpp
