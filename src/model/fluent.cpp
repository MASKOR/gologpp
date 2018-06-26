#include "fluent.h"
#include "execution.h"

namespace gologpp {


AbstractFluent::AbstractFluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Global(name, args)
, scope_(own_scope)
{ scope_->set_owner(dynamic_cast<Expression *>(this)); }

Scope &AbstractFluent::scope()
{ return *scope_; }

const Scope &AbstractFluent::scope() const
{ return *scope_; }

void AbstractFluent::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }


} // namespace gologpp
