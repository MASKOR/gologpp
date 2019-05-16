#include "language.h"
#include "reference.h"
#include "error.h"
#include "scope.h"

namespace gologpp {


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
	shared_ptr<Type> desired_type = global_scope().lookup_type(name);

	if (!type_)
		type_ = desired_type;

	return *type_ == *desired_type;
}


bool AbstractLanguageElement::set_type(const Type &t)
{
	if (!type_)
		type_ = t.shared_from_this();

	return *type_ == t;
}


template<class T>
void AbstractLanguageElement::ensure_type() {
	if (type().dynamic_tag() != T::tag())
		throw TypeError(*this, gologpp::type<T>());
}

} // namespace gologpp
