#include "Fluent.h"

namespace gologpp {
namespace generic {

Fluent::Fluent(const string &name, const vector<string> &args)
: Atom(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, args_(args)
, scope_(args, Scope::global_scope())
{}


Fluent::Fluent(Fluent &&other)
: Atom(std::move(other))
, Identifier(std::move(other))
, std::enable_shared_from_this<Fluent>(std::move(other))
, LanguageElement<Fluent>(std::move(other))
, args_(std::move(other.args_))
, scope_(std::move(other.scope_))
{}


const vector<string> &Fluent::args() const
{ return args_; }


Scope &Fluent::scope()
{ return scope_; }


const Scope &Fluent::scope() const
{ return scope_; }


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
