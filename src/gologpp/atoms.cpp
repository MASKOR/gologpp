#include "atoms.h"

namespace gologpp {


Expression::Expression(InScope &parent_expr)
: InScope(parent_expr)
{}

Expression::Expression(const shared_ptr<Scope> &parent_scope)
: InScope(parent_scope)
{}


Expression::Expression(Expression &&other)
: InScope(std::move(other))
{}


Expression::~Expression()
{}


Variable::Variable(const string &name, const shared_ptr<Scope> &parent_scope)
: Atom(parent_scope)
, Name(name)
{}


}