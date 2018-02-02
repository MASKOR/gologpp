#include <cctype>

#include "Formula.h"


namespace gologpp {
namespace generic {


Negation::Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: BooleanExpression(parent_scope)
, expression_(std::move(expression))
{}


Negation::members_t Negation::members()
{ return std::tie(*expression_); }


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , lhs_(lhs), op_(op), rhs_(rhs)
{}


Comparison::members_t Comparison::members()
{ return std::tie(*lhs_, *rhs_); }


ConnectiveFormula::ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, BooleanOperator op, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope)
: BooleanExpression(parent_scope)
, lhs_(std::move(lhs)), op_(op), rhs_(std::move(rhs))
{}


ConnectiveFormula::members_t ConnectiveFormula::members()
{ return std::tie(*lhs_, *rhs_); }


Quantification::Quantification(
        const shared_ptr<Variable> &variable,
        unique_ptr<BooleanExpression> &&expression,
        Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , variable_(variable), expression_(std::move(expression))
{}





} // namespace generic
} // namespace gologpp
