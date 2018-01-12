#include <cctype>

#include "Formula.h"


namespace gologpp {



Negation::Negation(const shared_ptr<Expression> &expression, InScope &parent_expr)
: Expression(parent_expr)
, expression_(expression)
{}


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, InScope &parent_expr)
    : Expression(parent_expr)
    , lhs_(lhs), op_(op), rhs_(rhs)
{}


ConnectiveFormula::ConnectiveFormula(const shared_ptr<Expression> &lhs, const shared_ptr<Expression> &rhs, InScope &parent_expr)
: Expression(parent_expr)
, lhs_(lhs), rhs_(rhs)
{}


Quantification::Quantification(
        const shared_ptr<Variable> &variable,
        const shared_ptr<Expression> &expression,
        InScope &parent_expr)
    : Expression(parent_expr)
    , variable_(variable), expression_(expression)
{}


} // namespace gologpp
