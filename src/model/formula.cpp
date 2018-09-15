#include <cctype>

#include "formula.h"
#include "expressions.h"
#include "atoms.h"


namespace gologpp {


Negation::Negation(BooleanExpression *expression)
: expression_(expression)
{ expression_->set_parent(this); }


const BooleanExpression &Negation::expression() const
{ return *expression_; }


string Negation::to_string(const string &pfx) const
{ return "!" + expression().to_string(pfx); }



string to_string(ComparisonOperator op)
{
	switch (op) {
	case NEQ:
		return "!=";
	case EQ:
		return "==";
	case GE:
		return ">=";
	case GT:
		return ">";
	case LE:
		return "<=";
	case LT:
		return "<";
	}
	return "[Unkown ComparisonOperator]";
}



Comparison::Comparison(NumericExpression *lhs, ComparisonOperator op, NumericExpression *rhs)
: lhs_(lhs)
, op_(op)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}


ComparisonOperator Comparison::op() const
{ return op_; }

const NumericExpression &Comparison::lhs() const
{ return *lhs_; }

const NumericExpression &Comparison::rhs() const
{ return *rhs_; }


string Comparison::to_string(const string &pfx) const
{ return '(' + lhs().to_string(pfx) + ' ' + gologpp::to_string(op()) + ' ' + rhs().to_string(pfx) + ')'; }



string to_string(BooleanOperator op)
{
	switch (op) {
	case AND:
		return "&";
	case OR:
		return "|";
	case IFF:
		return "==";
	case IMPLIES:
		return "->";
	case XOR:
		return "!=";
	}

	return "[Unknown BooleanOperator]";
}



BooleanOperation::BooleanOperation(
	BooleanExpression *lhs,
	BooleanOperator op,
	BooleanExpression *rhs
)
: lhs_(lhs)
, op_(op)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

BooleanOperator BooleanOperation::op() const
{ return op_; }

const BooleanExpression &BooleanOperation::lhs() const
{ return *lhs_; }

const BooleanExpression &BooleanOperation::rhs() const
{ return *rhs_; }

string BooleanOperation::to_string(const string &pfx) const
{ return '(' + lhs().to_string(pfx) + ' ' + gologpp::to_string(op()) + ' ' + rhs().to_string(pfx) + ')'; }



string to_string(QuantificationOperator op)
{
	switch (op) {
	case EXISTS:
		return "exists";
	case FORALL:
		return "forall";
	}

	return "[Unknown QuantificationOperator]";
}

Quantification::Quantification(
	Scope *own_scope,
	QuantificationOperator op,
	const shared_ptr<AbstractVariable> &variable,
	BooleanExpression *expression
)
:  ScopeOwner(own_scope)
, op_(op)
, variable_(variable)
, expression_(expression)
{
	std::dynamic_pointer_cast<Expression>(variable_)->set_parent(this);
	expression_->set_parent(this);
}


QuantificationOperator Quantification::op() const
{ return op_; }

const AbstractVariable &Quantification::variable() const
{ return *variable_; }

const BooleanExpression &Quantification::expression() const
{ return *expression_; }

string Quantification::to_string(const string &pfx) const
{ return linesep + pfx + gologpp::to_string(op()) + " (" + variable().to_string(pfx) + "): " + expression().to_string(pfx + indent); }



} // namespace gologpp
