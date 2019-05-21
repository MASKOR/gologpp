#include "language.h"
#include "reference.h"
#include "error.h"
#include "scope.h"

namespace gologpp {


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
	if (type() != gologpp::type<T>())
		throw TypeError(*this, gologpp::type<T>());
}

} // namespace gologpp
