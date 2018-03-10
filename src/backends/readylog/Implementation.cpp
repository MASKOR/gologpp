#include "Implementation.h"
#include "EffectAxiom.h"
#include "Action.h"
#include "Fluent.h"
#include "atoms.h"
#include "Formula.h"
#include "Procedure.h"
#include "Reference.h"
#include "Scope.h"
#include "arithmetic.h"

namespace gologpp {

#define DEFINE_MAKE_IMPL(GologT) \
unique_ptr<AbstractImplementation> ReadylogImplementor::make_impl(GologT &obj) \
{ return unique_ptr<AbstractImplementation>(new Implementation<GologT>(obj)); }

DEFINE_MAKE_IMPL(Action)
DEFINE_MAKE_IMPL(Reference<Action>)
DEFINE_MAKE_IMPL(EffectAxiom<BooleanExpression>)
DEFINE_MAKE_IMPL(EffectAxiom<ValueExpression>)
DEFINE_MAKE_IMPL(Fluent<BooleanExpression>)
DEFINE_MAKE_IMPL(Fluent<ValueExpression>)
DEFINE_MAKE_IMPL(Reference<Fluent<BooleanExpression>>)
DEFINE_MAKE_IMPL(Reference<Fluent<ValueExpression>>)
DEFINE_MAKE_IMPL(Scope)

DEFINE_MAKE_IMPL(Constant<BooleanExpression>)
DEFINE_MAKE_IMPL(Constant<ValueExpression>)
DEFINE_MAKE_IMPL(Variable<BooleanExpression>)
DEFINE_MAKE_IMPL(Variable<ValueExpression>)

DEFINE_MAKE_IMPL(ArithmeticOperation)

DEFINE_MAKE_IMPL(Negation)
DEFINE_MAKE_IMPL(Conjunction)
DEFINE_MAKE_IMPL(Disjunction)
DEFINE_MAKE_IMPL(ExistentialQuantification)
DEFINE_MAKE_IMPL(UniversalQuantification)

DEFINE_MAKE_IMPL(Block)
DEFINE_MAKE_IMPL(Choose)
DEFINE_MAKE_IMPL(Conditional)
DEFINE_MAKE_IMPL(Assignment<BooleanExpression>)
DEFINE_MAKE_IMPL(Assignment<ValueExpression>)
DEFINE_MAKE_IMPL(Pick)
DEFINE_MAKE_IMPL(Search)
DEFINE_MAKE_IMPL(Test)
DEFINE_MAKE_IMPL(While)
DEFINE_MAKE_IMPL(Procedure)
DEFINE_MAKE_IMPL(Reference<Procedure>)

DEFINE_MAKE_IMPL(Return<BooleanExpression>)
DEFINE_MAKE_IMPL(Return<ValueExpression>)

DEFINE_MAKE_IMPL(Function<BooleanExpression>)
DEFINE_MAKE_IMPL(Function<ValueExpression>)
DEFINE_MAKE_IMPL(Reference<Function<BooleanExpression>>)
DEFINE_MAKE_IMPL(Reference<Function<ValueExpression>>)

}
