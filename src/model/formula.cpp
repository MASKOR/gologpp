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


ComparisonOperator Comparison::op() const
{ return op_; }

const NumericExpression &Comparison::lhs() const
{ return *lhs_; }

const NumericExpression &Comparison::rhs() const
{ return *rhs_; }


BooleanOperation::BooleanOperation(BooleanExpression *lhs, BooleanOperator op, BooleanExpression *rhs, Scope &parent_scope)
: BooleanExpression(parent_scope)
, lhs_(lhs)
, op_(op)
, rhs_(rhs)
{}

BooleanOperator BooleanOperation::op() const
{ return op_; }

const BooleanExpression &BooleanOperation::lhs() const
{ return *lhs_; }

const BooleanExpression &BooleanOperation::rhs() const
{ return *rhs_; }


Quantification::Quantification(
	Scope *own_scope,
	QuantificationOperator op,
	const shared_ptr<AbstractVariable> &variable,
	BooleanExpression *expression
)
: BooleanExpression(own_scope->parent_scope())
, ScopeOwner(own_scope)
, op_(op)
, variable_(variable)
, expression_(expression)
{}


QuantificationOperator Quantification::op() const
{ return op_; }

const AbstractVariable &Quantification::variable() const
{ return *variable_; }

const BooleanExpression &Quantification::expression() const
{ return *expression_; }



} // namespace gologpp
