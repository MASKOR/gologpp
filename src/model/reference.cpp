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

#include "reference.h"

namespace gologpp {


AbstractReference::AbstractReference()
{}

AbstractReference::~AbstractReference()
{}


void AbstractReference::ensure_consistent()
{
	if (!consistent())
		throw UserError("Inconsistent reference: " + str());
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

gologpp::Reference<Variable>::Reference(const shared_ptr<Variable> &target)
: target_(target)
{}

gologpp::Reference<Variable>::Reference(Reference<Variable> &&other)
: target_(std::move(other.target_))
{}

Variable *gologpp::Reference<Variable>::operator ->()
{ return target().get(); }

const Variable &gologpp::Reference<Variable>::operator *() const
{ return *target(); }

Variable &gologpp::Reference<Variable>::operator *()
{ return *target(); }

const Variable *gologpp::Reference<Variable>::operator ->() const
{ return target().get(); }

const string &gologpp::Reference<Variable>::name() const
{ return target()->name(); }

bool gologpp::Reference<Variable>::bound() const
{ return target_.get(); }

shared_ptr<Variable> gologpp::Reference<Variable>::target()
{ return target_; }

shared_ptr<const Variable> gologpp::Reference<Variable>::target() const
{ return std::dynamic_pointer_cast<const Variable>(target_); }

bool gologpp::Reference<Variable>::operator ==(const Reference<Variable> &other) const
{ return *target() == *other.target(); }

bool gologpp::Reference<Variable>::operator !=(const Reference<Variable> &other) const
{ return !(*this == other); }

void gologpp::Reference<Variable>::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	semantics_ = implementor.make_semantics(*this);
}

bool gologpp::Reference<Variable>::consistent() const
{ return bound(); }

string gologpp::Reference<Variable>::to_string(const string &pfx) const
{ return target()->to_string(pfx); }

const Type &gologpp::Reference<Variable>::type() const
{ return target()->type(); }

size_t gologpp::Reference<Variable>::hash() const
{ return target()->hash(); }


}
