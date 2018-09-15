#include "atoms.h"

namespace gologpp {


AbstractVariable::AbstractVariable(const string &name)
: Identifier(name, 0)
{}


AbstractConstant::AbstractConstant(const string &representation)
: representation_(representation)
{}


const string &AbstractConstant::representation() const
{ return representation_; }


template<>
template<>
Constant<BooleanExpression>::Constant(bool v)
: AbstractConstant(v ? "true" : "false")
{}

template<>
template<>
Constant<NumericExpression>::Constant(const string &representation)
: AbstractConstant(representation)
{}



}
