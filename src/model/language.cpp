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

void AbstractLanguageElement::set_implementation(unique_ptr<AbstractSemantics> &&impl)
{ semantics_ = std::move(impl); }

const Type &AbstractLanguageElement::type() const
{ return *type_; }


bool AbstractLanguageElement::set_type_by_name(const string &name)
{
	shared_ptr<const Type> desired_type = global_scope().lookup_type(name);

	if (!type())
		type_ = desired_type;

	return type() == *desired_type;
}


bool AbstractLanguageElement::set_type(const Type &t)
{
	if (!type())
		type_ = t.shared_from_this();

	return type() == t;
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


} // namespace gologpp
