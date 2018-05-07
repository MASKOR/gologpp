#include "atoms.h"

namespace gologpp {


AbstractVariable::AbstractVariable(const string &name, Scope &parent_scope)
: Name(name)
, parent_scope_(parent_scope)
{}


AbstractConstant::AbstractConstant(const string &representation)
: representation_(representation)
{}


const string &AbstractConstant::representation() const
{ return representation_; }

}
