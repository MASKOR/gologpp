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

#define DEFINE_MAKE_IMPL(GologT) \
unique_ptr<AbstractImplementation> ReadylogImplementor::make_impl(GologT &obj) \
{ return unique_ptr<AbstractImplementation>(new Implementation<GologT>(obj)); }

DEFINE_MAKE_IMPL(Action)
DEFINE_MAKE_IMPL(Reference<Action>)
DEFINE_MAKE_IMPL(EffectAxiom)
DEFINE_MAKE_IMPL(Fluent)
DEFINE_MAKE_IMPL(Reference<Fluent>)
DEFINE_MAKE_IMPL(Scope)

DEFINE_MAKE_IMPL(AnyValue)
DEFINE_MAKE_IMPL(Variable)

DEFINE_MAKE_IMPL(Negation)
DEFINE_MAKE_IMPL(Conjunction)
DEFINE_MAKE_IMPL(Disjunction)
DEFINE_MAKE_IMPL(ExistentialQuantification)
DEFINE_MAKE_IMPL(UniversalQuantification)

DEFINE_MAKE_IMPL(Block)
DEFINE_MAKE_IMPL(Choose)
DEFINE_MAKE_IMPL(Conditional)
DEFINE_MAKE_IMPL(Assignment)
DEFINE_MAKE_IMPL(Pick)
DEFINE_MAKE_IMPL(Search)
DEFINE_MAKE_IMPL(Test)
DEFINE_MAKE_IMPL(While)
DEFINE_MAKE_IMPL(Procedure)
DEFINE_MAKE_IMPL(Reference<Procedure>)

}
