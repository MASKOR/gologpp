#include "language.h"
#include "reference.h"

namespace gologpp {


bool AbstractLanguageElement::is_ref() const
{ return dynamic_cast<const AbstractReference *>(this); }

Scope &AbstractLanguageElement::m_scope()
{ return scope(); }

string AbstractLanguageElement::str() const
{ return to_string(""); }

void AbstractLanguageElement::set_implementation(unique_ptr<AbstractSemantics> &&impl)
{ semantics_ = std::move(impl); }



} // namespace gologpp