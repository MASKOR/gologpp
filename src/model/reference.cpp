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


Reference<Variable>::Reference(const shared_ptr<Variable> &target)
: target_(target)
{}

Reference<Variable>::Reference(Reference<Variable> &&other)
: target_(std::move(other.target_))
{}

Variable *Reference<Variable>::operator ->()
{ return target().get(); }

const Variable &Reference<Variable>::operator *() const
{ return *target(); }

Variable &Reference<Variable>::operator *()
{ return *target(); }

const Variable *Reference<Variable>::operator ->() const
{ return target().get(); }

const string &Reference<Variable>::name() const
{ return target()->name(); }

bool Reference<Variable>::bound() const
{ return target_.get(); }

shared_ptr<Variable> Reference<Variable>::target()
{ return target_; }

shared_ptr<const Variable> Reference<Variable>::target() const
{ return std::dynamic_pointer_cast<const Variable>(target_); }

bool Reference<Variable>::operator ==(const Reference<Variable> &other) const
{ return *target() == *other.target(); }

bool Reference<Variable>::operator !=(const Reference<Variable> &other) const
{ return !(*this == other); }

void Reference<Variable>::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	semantics_ = implementor.make_semantics(*this);
}

bool Reference<Variable>::consistent() const
{ return bound(); }

string Reference<Variable>::to_string(const string &pfx) const
{ return target()->to_string(pfx); }

const Type &Reference<Variable>::type() const
{ return target()->type(); }

size_t Reference<Variable>::hash() const
{ return target()->hash(); }

const Expression &Reference<Variable>::arg_for_param(shared_ptr<const Variable>) const
{ throw Bug("This method is undefined and should not have been called"); }


}
