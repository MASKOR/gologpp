#include "string.h"


namespace gologpp {


ToString::ToString(Expression *expr)
: expr_(expr)
{}

const Expression &ToString::expression() const
{ return *expr_; }

string ToString::to_string(const string &pfx) const
{ return pfx + "to_string(" + expr_->str() + ")"; }



StringConcatenation::StringConcatenation(Expression *lhs, Expression *rhs)
: lhs_(lhs)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}


const Expression &StringConcatenation::lhs() const
{ return *lhs_; }

const Expression &StringConcatenation::rhs() const
{ return *rhs_; }

string StringConcatenation::to_string(const string &pfx) const
{ return pfx + lhs().to_string(pfx) + " + " + rhs().to_string(pfx); }



}
