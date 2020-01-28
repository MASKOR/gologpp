/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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
	if (!(lhs->type() >= *rhs
		|| lhs->type() <= rhs->type()
		|| rhs->type() >= *lhs
		|| rhs->type() <= lhs->type()
	) )
		throw TypeError("Incompatible types in comparison: "
			+ lhs->type().name() + " and " + rhs->type().name()
		);
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
	case ComparisonOperator::NEQ:
		return "!=";
	case ComparisonOperator::EQ:
		return "==";
	case ComparisonOperator::GE:
		return ">=";
	case ComparisonOperator::GT:
		return ">";
	case ComparisonOperator::LE:
		return "<=";
	case ComparisonOperator::LT:
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
	case BooleanOperator::AND:
		return "&";
	case BooleanOperator::OR:
		return "|";
	case BooleanOperator::IFF:
		return "==";
	case BooleanOperator::IMPLIES:
		return "->";
	case BooleanOperator::XOR:
		return "!=";
	}

	return "[Unknown BooleanOperator]";
}


unsigned int precedence(BooleanOperation::Operator op)
{
	switch (op) {
	case BooleanOperator::IMPLIES:
	case BooleanOperator::XOR:
	case BooleanOperator::IFF:
		return 0;
	case BooleanOperator::OR:
		return 1;
	case BooleanOperator::AND:
		return 2;
	}
	throw Bug("Undefined boolean operator: " + std::to_string(op));
};



string to_string(QuantificationOperator op)
{
	switch (op) {
	case QuantificationOperator::EXISTS:
		return "exists";
	case QuantificationOperator::FORALL:
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
