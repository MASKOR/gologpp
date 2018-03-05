#include "Fluent.h"

namespace gologpp {

Fluent::Fluent(const string &name, const vector<string> &args)
: BooleanExpression(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, args_(args)
, scope_(args, Scope::global_scope())
{}


const vector<string> &Fluent::args() const
{ return args_; }


Scope &Fluent::scope()
{ return scope_; }


const Scope &Fluent::scope() const
{ return scope_; }


FunctionalFluent::FunctionalFluent(const string &name, const vector<string> &args)
: ValueExpression(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, args_(args)
, scope_(args, Scope::global_scope())
{}


const vector<string> &FunctionalFluent::args() const
{ return args_; }


Scope &FunctionalFluent::scope()
{ return scope_; }


const Scope &FunctionalFluent::scope() const
{ return scope_; }


} // namespace gologpp
