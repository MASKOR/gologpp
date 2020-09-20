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

#include "expressions.h"
#include "scope.h"


namespace gologpp {

Expression::Expression()
{}

Scope &Expression::parent_scope()
{ return parent()->scope(); }

const Scope &Expression::parent_scope() const
{ return parent()->scope(); }

bool Expression::operator <= (const Type &t) const
{ return t >= *this; }

bool Expression::operator ==(const Expression &other) const
{ return this == &other; }

bool Expression::operator !=(const Expression &other) const
{ return this != &other; }



Instruction::Instruction()
{}

Scope &Instruction::parent_scope()
{ return parent()->scope(); }

const Scope &Instruction::parent_scope() const
{ return parent()->scope(); }



}
