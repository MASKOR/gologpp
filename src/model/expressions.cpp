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
: parent_(nullptr)
{}

Scope &Expression::parent_scope()
{ return parent_->scope(); }

const Scope &Expression::parent_scope() const
{ return parent_->scope(); }

ModelElement *Expression::parent()
{ return parent_; }

const ModelElement *Expression::parent() const
{ return parent_; }

void Expression::set_parent(AbstractLanguageElement *parent)
{ parent_ = parent; }

bool Expression::operator <= (const Type &t) const
{ return t >= *this; }


Instruction::Instruction()
: parent_(nullptr)
{}

Scope &Instruction::parent_scope()
{ return parent_->scope(); }

const Scope &Instruction::parent_scope() const
{ return parent_->scope(); }

ModelElement *Instruction::parent()
{ return parent_; }

const ModelElement *Instruction::parent() const
{ return parent_; }

void Instruction::set_parent(AbstractLanguageElement *parent)
{ parent_ = parent; }



}
