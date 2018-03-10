#include "Fluent.h"

namespace gologpp {


AbstractFluent::AbstractFluent(const string &name, const vector<string> &args)
: Expression(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, scope_(this, {}, Scope::global_scope())
, args_(args)
{}


const vector<string> &AbstractFluent::args() const
{ return args_; }


shared_ptr<AbstractVariable> AbstractFluent::argument(arity_t idx) const
{ return scope_.variable(args_[static_cast<size_t>(idx)]); }


Scope &AbstractFluent::scope()
{ return scope_; }


const Scope &AbstractFluent::scope() const
{ return scope_; }



} // namespace gologpp
