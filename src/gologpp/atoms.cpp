#include "atoms.h"

namespace gologpp {


AbstractVariable::AbstractVariable(const string &name, Scope &parent_scope)
: Name(name)
, parent_scope_(parent_scope)
{}


}
