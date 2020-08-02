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


Binding::Binding(const Binding &other)
{
	for (auto &pair : other.var_bindings_)
		var_bindings_.emplace(pair.first, *dynamic_cast<const Value &>(pair.second.get()).copy());
	if (other.semantics_)
		set_semantics(unique_ptr<GeneralSemantics<ModelElement>>(
			other.general_semantics<Binding>().copy(*this)
		) );
}

Binding::Binding(Binding &&other)
: var_bindings_(std::move(other.var_bindings_))
{
	if (other.semantics_)
		throw Bug("Cannot move a Binding after semantics have been assigned");
}

void Binding::bind(shared_ptr<const Variable> var, Expression &expr)
{ var_bindings_.insert(std::make_pair(var, std::ref(expr))); }

Expression &Binding::get(shared_ptr<const Variable> param) const
{
	auto it = var_bindings_.find(param);
	if (it == var_bindings_.end())
		throw Bug("No parameter by the name " + param->str());
	return it->second;
}

const Binding::MapT &Binding::map() const
{ return var_bindings_; }


void Binding::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		set_semantics(f.make_semantics(*this));
		for (auto &entry : var_bindings_)
			entry.second.get().attach_semantics(f);
	}
}


string Binding::to_string(const string &pfx) const
{
	string rv;
	for (auto &entry : var_bindings_)
		rv += entry.first->to_string(pfx) + "=" + entry.second.get().to_string(pfx) + ", ";
	if (!rv.empty())
		rv = rv.substr(0, rv.length() - 2);
	return rv;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

GeneralSemantics<Binding>::GeneralSemantics(const Binding &elem, AExecutionContext &context)
: element_(&elem)
, context_(context)
{}

const Binding &GeneralSemantics<Binding>::element() const
{ return *element_; }

void GeneralSemantics<Binding>::update_element(const Binding *new_element)
{ element_ = new_element; }

AExecutionContext &GeneralSemantics<Binding>::context() const
{ return context_; }



void Reference<Variable>::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	semantics_ = implementor.make_semantics(*this);
}



}
