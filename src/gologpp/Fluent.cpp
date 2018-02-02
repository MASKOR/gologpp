#include "Fluent.h"

namespace gologpp {
namespace generic {

Fluent::Fluent(const string &name, const vector<string> &args)
: Atom(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
{}


tuple<> Fluent::members()
{ return std::tie(); }


Initially::Initially(Reference<Fluent> &&fluent, unique_ptr<AnyValue> &&value)
: fluent_(std::move(fluent))
, value_(std::move(value))
{}


tuple<Reference<Fluent> &, AnyValue &> Initially::members()
{ return std::tie(fluent_, *value_); }


const Fluent &Initially::fluent() const
{ return *fluent_; }

const AnyValue &Initially::initial_value() const
{ return *value_; }


} // namespace generic
} // namespace gologpp
