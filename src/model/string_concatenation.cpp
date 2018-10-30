#include "string_concatenation.h"


namespace gologpp {

StringConcatenation::StringConcatenation(StringExpression *lhs, StringExpression *rhs)
: lhs_(lhs)
, rhs_(rhs)
{
	lhs_->set_parent(this);
	rhs_->set_parent(this);
}


const StringExpression &StringConcatenation::lhs() const
{ return *lhs_; }

const StringExpression &StringConcatenation::rhs() const
{ return *rhs_; }

string StringConcatenation::to_string(const string &pfx) const
{ return pfx + lhs().to_string(pfx) + " + " + rhs().to_string(pfx); }



}