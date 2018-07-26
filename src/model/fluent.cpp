#include "fluent.h"
#include "execution.h"

namespace gologpp {


AbstractFluent::AbstractFluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
{}

void AbstractFluent::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }


} // namespace gologpp
