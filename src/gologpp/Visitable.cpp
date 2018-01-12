#include "Visitable.h"

namespace gologpp {

Visitable::~Visitable()
{}


void Visitable::accept(Visitor &visitor)
{ visitor.visit(*this); }

Visitor::~Visitor()
{}



}