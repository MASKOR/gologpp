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

#include "language.h"
#include "reference.h"
#include "error.h"
#include "scope.h"

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


AbstractLanguageElement::AbstractLanguageElement(shared_ptr<const UndefinedType> t)
: type_(t)
{}

AbstractLanguageElement::AbstractLanguageElement()
: type_(gologpp::type<UndefinedType>().shared_from_this())
{}

bool AbstractLanguageElement::is_ref() const
{ return dynamic_cast<const AbstractReference *>(this); }

Scope &AbstractLanguageElement::m_scope()
{ return scope(); }

string AbstractLanguageElement::str() const
{ return to_string(""); }

void AbstractLanguageElement::set_semantics(unique_ptr<AbstractSemantics> &&impl)
{ semantics_ = std::move(impl); }

const Type &AbstractLanguageElement::type() const
{ return *type_; }


void AbstractLanguageElement::set_type_by_name(const string &name)
{
	shared_ptr<const Type> desired_type = global_scope().lookup_type(name);

	if (!type())
		type_ = desired_type;

	if (type() != *desired_type)
		throw TypeError("Cannot override type " + type().name() + " of `" + str() + "' with " + name);
}


void AbstractLanguageElement::set_type(const Type &t)
{
	if (!type())
		type_ = t.shared_from_this();

	if (type() != t)
		throw TypeError("Cannot override type " + type().name() + " of `" + str() + "' with " + t.name());
}


template<class T>
void AbstractLanguageElement::ensure_type() {
	const Type &this_type = type();
	if (typeid(this_type) != typeid(T))
		throw TypeError(this->str() + " is not of type " + T::name());
}

#define GOLOGPP_INSTANTIATE_ENSURE_TYPE(_r, _data, T) \
	template \
	void AbstractLanguageElement::ensure_type<T>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_INSTANTIATE_ENSURE_TYPE, (), GOLOGPP_TYPES)


void AbstractLanguageElement::ensure_type(const Type &t) {
	if (!type())
		type_ = t.shared_from_this();

	if (type() && type() != t)
		throw TypeError(this->str() + " has type " + type().name()
			+ " and cannot be redefined to type " + t.name()
		);
}


void AbstractLanguageElement::set_type_unchecked(const string &name)
{ type_ = global_scope().lookup_type(name); }


} // namespace gologpp
