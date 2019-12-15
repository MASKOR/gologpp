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

void AbstractLanguageElement::set_semantics(unique_ptr<AbstractSemantics<AbstractLanguageElement>> &&impl)
{ semantics_ = std::move(impl); }

const Type &AbstractLanguageElement::type() const
{ return *type_; }

shared_ptr<const Type> AbstractLanguageElement::type_ptr() const
{ return this->type().shared_from_this(); }


void AbstractLanguageElement::set_type(const Type &t)
{
	if (!type())
		type_ = t.shared_from_this();

	if (!(type() >= t))
		throw TypeError("Cannot override type " + type().name() + " of `" + str() + "' with " + t.name());
}


template<class T>
void AbstractLanguageElement::ensure_type() {
	ensure_type(gologpp::type<T>());
}

template<>
void AbstractLanguageElement::ensure_type<ListType>() {
	if (!type().is<ListType>())
		throw TypeError("Expression " + str() + " is of type " + type().name()
			+ ", which is not a list type");
}

template<>
void AbstractLanguageElement::ensure_type<CompoundType>() {
	if (!type().is<CompoundType>())
		throw TypeError("Expression " + str() + " is of type " + type().name()
			+ ", which is not a compound type");
}

#define GOLOGPP_INSTANTIATE_ENSURE_TYPE(_r, _data, T) \
	template \
	void AbstractLanguageElement::ensure_type<T>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_INSTANTIATE_ENSURE_TYPE, (), GOLOGPP_PREDEFINED_TYPES)


void AbstractLanguageElement::ensure_type(const Type &t) {
	if (!type())
		type_ = t.shared_from_this();

	if (!(type() >= *this))
		throw TypeError(this->str() + " has type " + type().name()
			+ " and cannot be redefined to type " + t.name()
		);
}




void AbstractLanguageElement::set_type_unchecked(const Type &t)
{ type_ = t.shared_from_this(); }


template<class T>
void AbstractLanguageElement::t_set_type_unchecked()
{ type_ = get_type<T>().shared_from_this(); }

#define GOLOGPP_INSTANTIATE_T_SET_TYPE_UNCHECKED(_r, _data, T) \
	template \
	void AbstractLanguageElement::t_set_type_unchecked<T>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_INSTANTIATE_T_SET_TYPE_UNCHECKED, (), GOLOGPP_PREDEFINED_TYPES)


} // namespace gologpp
