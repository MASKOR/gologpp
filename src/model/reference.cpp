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


GeneralSemantics<Binding<Value> >::GeneralSemantics(const Binding<Value> &elem, ExecutionContext &context)
: element_(&elem)
, context_(context)
{}

const Binding<Value> &GeneralSemantics<Binding<Value> >::element() const
{ return *element_; }

void GeneralSemantics<Binding<Value> >::update_element(const Binding<Value> *new_element)
{ element_ = new_element; }

ExecutionContext &GeneralSemantics<Binding<Value> >::context() const
{ return context_; }



void Reference<Variable>::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	semantics_ = implementor.make_semantics(*this);
}



}
