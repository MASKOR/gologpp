#include "atoms.h"

namespace gologpp {
namespace generic {


Expression::Expression(Scope &parent_scope)
: parent_scope_(parent_scope)
{}


Scope &Expression::parent_scope()
{ return parent_scope_; }


Variable::Variable(const string &name, Scope &parent_scope)
: Atom(parent_scope)
, Name(name)
{}


tuple<> Variable::members()
{ return std::tie(); }


AnyValue::AnyValue()
: Atom(Scope::global_scope())
{}


AnyValue::members_t AnyValue::members()
{ return std::tie(); }

}
}