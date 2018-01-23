#include <cctype>

#include "Formula.h"


namespace gologpp {
namespace generic {


Negation::Negation(const shared_ptr<BooleanExpression> &expression, const shared_ptr<Scope> &parent_scope)
: BooleanExpression(parent_scope)
, expression_(expression)
{}


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, const shared_ptr<Scope> &parent_scope)
    : BooleanExpression(parent_scope)
    , lhs_(lhs), op_(op), rhs_(rhs)
{}


ConnectiveFormula::ConnectiveFormula(const shared_ptr<BooleanExpression> &lhs, const shared_ptr<BooleanExpression> &rhs, const shared_ptr<Scope> &parent_scope)
: BooleanExpression(parent_scope)
, lhs_(lhs), rhs_(rhs)
{}


Quantification::Quantification(
        const shared_ptr<Variable> &variable,
        const shared_ptr<BooleanExpression> &expression,
        const shared_ptr<Scope> &parent_scope)
    : BooleanExpression(parent_scope)
    , variable_(variable), expression_(expression)
{}


} // namespace generic
} // namespace gologpp
