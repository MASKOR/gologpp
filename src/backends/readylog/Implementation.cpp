#include "Implementation.h"
#include "EffectAxiom.h"
#include "Action.h"
#include "Fluent.h"
#include "atoms.h"
#include "Formula.h"
#include "Procedure.h"
#include "Reference.h"
#include "Scope.h"

namespace gologpp {

#define DEFINE_GET_IMPL(GologT) \
unique_ptr<AbstractImplementation> ReadylogImplementor::get_impl(generic::GologT &obj) \
{ return unique_ptr<AbstractImplementation>(new Implementation<generic::GologT>(obj)); }

DEFINE_GET_IMPL(Action)
DEFINE_GET_IMPL(Reference<generic::Action>)
DEFINE_GET_IMPL(EffectAxiom)
DEFINE_GET_IMPL(Fluent)
DEFINE_GET_IMPL(Reference<generic::Fluent>)
DEFINE_GET_IMPL(Scope)

DEFINE_GET_IMPL(AnyValue)
DEFINE_GET_IMPL(Variable)

DEFINE_GET_IMPL(Negation)
DEFINE_GET_IMPL(Conjunction)
DEFINE_GET_IMPL(Disjunction)
DEFINE_GET_IMPL(ExistentialQuantification)
DEFINE_GET_IMPL(UniversalQuantification)

DEFINE_GET_IMPL(Block)
DEFINE_GET_IMPL(Choose)
DEFINE_GET_IMPL(Conditional)
DEFINE_GET_IMPL(Assignment)
DEFINE_GET_IMPL(Pick)
DEFINE_GET_IMPL(Search)
DEFINE_GET_IMPL(Test)
DEFINE_GET_IMPL(While)
DEFINE_GET_IMPL(Procedure)
DEFINE_GET_IMPL(Reference<generic::Procedure>)

}
