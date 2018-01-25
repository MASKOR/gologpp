#include "Fluent.h"

namespace gologpp {
namespace generic {

Fluent::Fluent(const string &name, const vector<string> &args)
: Atom(Scope::global_scope())
, NameWithArity(name, static_cast<arity_t>(args.size()))
, LanguageElement(*this)
{}


Initially::Initially(Reference<Fluent> &&fluent, unique_ptr<AnyValue> &&value)
: LanguageElement(*this)
, fluent_(std::move(fluent))
, value_(std::move(value))
{}


const Fluent &Initially::fluent() const
{ return *fluent_; }

const AnyValue &Initially::initial_value() const
{ return *value_; }


} // namespace generic
} // namespace gologpp
