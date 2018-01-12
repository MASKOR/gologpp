#include "Fluent.h"

namespace gologpp {

Fluent::Fluent(const string &name, const vector<string> &args)
: Atom(Scope::global_scope())
, NameWithArity(name, static_cast<arity_t>(args.size()))
{}


}
