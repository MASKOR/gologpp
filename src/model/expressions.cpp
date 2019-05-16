#include "expressions.h"
#include "scope.h"


namespace gologpp {

Expression::Expression()
: parent_(nullptr)
{}

Scope &Expression::parent_scope()
{ return parent_->scope(); }

const Scope &Expression::parent_scope() const
{ return parent_->scope(); }

AbstractLanguageElement *Expression::parent()
{ return parent_; }

const AbstractLanguageElement *Expression::parent() const
{ return parent_; }

void Expression::set_parent(AbstractLanguageElement *parent)
{ parent_ = parent; }




}
