#include <cctype>

#include "Formula.h"


namespace gologpp {
namespace generic {


BooleanExpression::BooleanExpression(BooleanExpression &&x)
: Expression(std::move(x))
{}

Negation::Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: BooleanExpression(parent_scope)
, expression_(std::move(expression))
{}


const BooleanExpression &Negation::expression() const
{ return *expression_; }


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , lhs_(lhs), op_(op), rhs_(rhs)
{}


ConnectiveFormula::ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope)
: BooleanExpression(parent_scope)
, lhs_(std::move(lhs)), rhs_(std::move(rhs))
{}


Quantification::Quantification(
        const shared_ptr<Variable> &variable,
        unique_ptr<BooleanExpression> &&expression,
        Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , variable_(variable), expression_(std::move(expression))
{}



} // namespace generic
} // namespace gologpp
