#include <cctype>

#include "formula.h"
#include "expressions.h"
#include "atoms.h"


namespace gologpp {


Negation::Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: BooleanExpression(parent_scope)
, expression_(std::move(expression))
{}


const BooleanExpression &Negation::expression() const
{ return *expression_; }


Comparison::Comparison(NumericExpression *lhs, ComparisonOperator op, NumericExpression *rhs, Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , lhs_(lhs)
    , op_(op)
    , rhs_(rhs)
{}


ComparisonOperator Comparison::cmp_operator() const
{ return op_; }

const NumericExpression &Comparison::lhs() const
{ return *lhs_; }

const NumericExpression &Comparison::rhs() const
{ return *rhs_; }


ConnectiveFormula::ConnectiveFormula(unique_ptr<BooleanExpression> &&lhs, unique_ptr<BooleanExpression> &&rhs, Scope &parent_scope)
: BooleanExpression(parent_scope)
, lhs_(std::move(lhs)), rhs_(std::move(rhs))
{}


const BooleanExpression &ConnectiveFormula::lhs() const
{ return *lhs_; }


const BooleanExpression &ConnectiveFormula::rhs() const
{ return *rhs_; }


Quantification::Quantification(
        const shared_ptr<AbstractVariable> &variable,
        unique_ptr<BooleanExpression> &&expression,
        Scope &parent_scope)
    : BooleanExpression(parent_scope)
    , variable_(variable), expression_(std::move(expression))
{}


const AbstractVariable &Quantification::variable() const
{ return *variable_; }


const BooleanExpression &Quantification::expression() const
{ return *expression_; }



} // namespace gologpp
