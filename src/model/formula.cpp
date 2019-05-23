#include <cctype>

#include "formula.h"
#include "expressions.h"


namespace gologpp {


Negation::Negation(Expression *expression)
: expression_(expression)
{ expression_->set_parent(this); }


const Expression &Negation::expression() const
{ return *expression_; }


string Negation::to_string(const string &pfx) const
{ return "!" + expression().to_string(pfx); }



Comparison::Comparison(Expression *lhs, ComparisonOperator op, Expression *rhs)
: lhs_(lhs)
, op_(op)
, rhs_(rhs)
{
	ensure_type_equality(*lhs, *rhs);
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}

ComparisonOperator Comparison::op() const
{ return op_; }

const Expression &Comparison::lhs() const
{ return *lhs_; }

const Expression &Comparison::rhs() const
{ return *rhs_; }

string Comparison::to_string(const string &pfx) const
{ return '(' + lhs().to_string(pfx) + ' ' + gologpp::to_string(op()) + ' ' + rhs().to_string(pfx) + ')'; }


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



BooleanOperation::BooleanOperation(
	Expression *lhs,
	BooleanOperator op,
	Expression *rhs
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

const Expression &BooleanOperation::lhs() const
{ return *lhs_; }

const Expression &BooleanOperation::rhs() const
{ return *rhs_; }

string BooleanOperation::to_string(const string &pfx) const
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
	const shared_ptr<Variable> &variable,
	Expression *expression
)
:  ScopeOwner(own_scope)
, op_(op)
, variable_(variable)
, expression_(expression)
{
	variable_->set_parent(this);
	expression_->set_parent(this);
}


QuantificationOperator Quantification::op() const
{ return op_; }

const Variable &Quantification::variable() const
{ return *variable_; }

const Expression &Quantification::expression() const
{ return *expression_; }

string Quantification::to_string(const string &pfx) const
{ return linesep + pfx + gologpp::to_string(op()) + " (" + variable().to_string(pfx) + "): " + expression().to_string(pfx + indent); }



} // namespace gologpp
