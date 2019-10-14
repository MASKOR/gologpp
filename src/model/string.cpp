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
