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


const AnyValue::variant_t &AnyValue::value() const
{ return value_; }


}
}