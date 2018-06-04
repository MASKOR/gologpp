#include "atoms.h"

namespace gologpp {


AbstractVariable::AbstractVariable(const string &name, Scope &parent_scope)
: Expression(parent_scope)
, Identifier(name, 0)
{}


AbstractConstant::AbstractConstant(const string &representation)
: representation_(representation)
{}


const string &AbstractConstant::representation() const
{ return representation_; }

}
