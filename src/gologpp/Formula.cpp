#include <cctype>

#include "Formula.h"


namespace gologpp {
namespace generic {


Negation::Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: BooleanExpression(parent_scope)
, LanguageElement(*this)
, expression_(std::move(expression))
{}


Comparison::Comparison(const shared_ptr<Atom> &lhs, ComparisonOperator op, const shared_ptr<Atom> &rhs, Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , LanguageElement(*this)
    , lhs_(lhs), op_(op), rhs_(rhs)
{}


ConnectiveFormula::ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, BooleanOperator op, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope)
: BooleanExpression(parent_scope)
, LanguageElement(*this)
, lhs_(std::move(lhs)), op_(op), rhs_(std::move(rhs))
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
