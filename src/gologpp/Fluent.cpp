#include "Fluent.h"

namespace gologpp {
namespace generic {

Fluent::Fluent(const string &name, const vector<string> &args)
: Atom(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, args_(args)
, scope_(args, Scope::global_scope())
{}


vector<shared_ptr<Expression>> Fluent::args() const
{ return scope_.variables(args_); }


Initially::Initially(Reference<Fluent> &&fluent, unique_ptr<AnyValue> &&value)
: fluent_(std::move(fluent))
, value_(std::move(value))
{}


const Fluent &Initially::fluent() const
{ return *fluent_; }

const AnyValue &Initially::initial_value() const
{ return *value_; }


} // namespace generic
} // namespace gologpp
