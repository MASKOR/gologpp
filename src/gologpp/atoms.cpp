#include "atoms.h"

namespace gologpp {
namespace generic {


Expression::Expression(Expression &&x)
: parent_scope_(std::move(x).parent_scope_)
{}


Expression::Expression(Scope &parent_scope)
: parent_scope_(parent_scope)
{}


Expression::~Expression()
{}


Atom::Atom(Atom &&other)
: Expression(std::move(other))
{}


Scope &Expression::parent_scope()
{ return parent_scope_; }


const Scope &Expression::parent_scope() const
{ return parent_scope_; }


Variable::Variable(const string &name, Scope &parent_scope)
: Atom(parent_scope)
, Name(name)
{}


const AnyValue::variant_t &AnyValue::value() const
{ return value_; }


}
}
