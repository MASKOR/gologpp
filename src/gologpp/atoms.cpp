#include "atoms.h"

namespace gologpp {
namespace generic {


Expression::Expression(const shared_ptr<Scope> &parent_scope)
: parent_scope_(parent_scope)
{}

Expression::Expression(Expression &&other)
: parent_scope_(std::move(other.parent_scope_))
{}

Expression::~Expression()
{}


shared_ptr<Scope> Expression::parent_scope()
{ return parent_scope_; }

Expression::operator shared_ptr<Scope> ()
{ return parent_scope(); }


Variable::Variable(const string &name, const shared_ptr<Scope> &parent_scope)
: Atom(parent_scope)
, Name(name)
{}


AnyValue::AnyValue()
: Atom(Scope::global_scope())
{}


}
}