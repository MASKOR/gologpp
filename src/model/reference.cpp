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
#include "value.h"

#include <unordered_map>


namespace gologpp {

template<>
Binding<Value>::Binding(const Binding<Value> &other)
{
	for (auto &pair : other.var_bindings_)
		var_bindings_.emplace(pair.first, new Value(*pair.second));
	if (other.semantics_)
		set_semantics(unique_ptr<GeneralSemantics<ModelElement>>(
			other.general_semantics<Binding<Value>>().copy(*this)
		) );
}

template<class ExprT>
Binding<ExprT>::Binding(Binding<ExprT> &&other)
: var_bindings_(std::move(other.var_bindings_))
{
	if (other.semantics_)
		throw Bug("Cannot move a Binding after semantics have been assigned");
}

template<class ExprT>
void Binding<ExprT>::bind(shared_ptr<const Variable> var, unique_ptr<ExprT> &&expr)
{ var_bindings_.insert(std::make_pair(var, std::move(expr))); }

template<class ExprT>
ExprT &Binding<ExprT>::get(shared_ptr<const Variable> param) const
{
	auto it = var_bindings_.find(param);
	if (it == var_bindings_.end())
		throw Bug("No parameter by the name " + param->str());
	return *it->second;
}

template<class ExprT>
const typename Binding<ExprT>::MapT &Binding<ExprT>::map() const
{ return var_bindings_; }


template<class ExprT>
void Binding<ExprT>::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		set_semantics(f.make_semantics(*this));
		for (auto &entry : var_bindings_)
			entry.second->attach_semantics(f);
	}
}


template<class ExprT>
string Binding<ExprT>::to_string(const string &pfx) const
{
	string rv;
	for (auto &entry : var_bindings_)
		rv += entry.first->to_string(pfx) + "=" + entry.second->to_string(pfx) + ", ";
	if (!rv.empty())
		rv = rv.substr(0, rv.length() - 2);
	return rv;
}


template
class Binding<Expression>;

template
class Binding<Value>;

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class ExprT>
GeneralSemantics<Binding<ExprT>>::GeneralSemantics(const Binding<ExprT> &elem, AExecutionController &context)
: element_(&elem)
, context_(context)
{}

template<class ExprT>
const Binding<ExprT> &GeneralSemantics<Binding<ExprT>>::element() const
{ return *element_; }

template<class ExprT>
void GeneralSemantics<Binding<ExprT>>::update_element(const Binding<ExprT> *new_element)
{ element_ = new_element; }

template<class ExprT>
AExecutionController &GeneralSemantics<Binding<ExprT>>::context() const
{ return context_; }


template<class ExprT>
const ModelElement &GeneralSemantics<Binding<ExprT>>::model_element() const
{ return *element_; }

template
class GeneralSemantics<Binding<Expression>>;

template
class GeneralSemantics<Binding<Value>>;

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void Reference<Variable>::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	semantics_ = implementor.make_semantics(*this);
}



}
