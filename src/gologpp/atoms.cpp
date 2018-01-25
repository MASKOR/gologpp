#include "atoms.h"

namespace gologpp {
namespace generic {


Expression::Expression(Scope &parent_scope)
: parent_scope_(parent_scope)
{}

Expression::Expression(Expression &&other)
: parent_scope_(other.parent_scope_)
{}

Expression::~Expression()
{}


Scope &Expression::parent_scope()
{ return parent_scope_; }

Expression::operator Scope & ()
{ return parent_scope(); }


Variable::Variable(const string &name, Scope &parent_scope)
: Atom(parent_scope)
, Name(name)
, LanguageElement(*this)
{}


AnyValue::AnyValue()
: Atom(Scope::global_scope())
, LanguageElement(*this)
{}


}
}