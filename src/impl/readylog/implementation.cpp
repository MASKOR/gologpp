#include "implementation.h"
#include "effect_axiom.h"
#include "action.h"
#include "fluent.h"
#include "atoms.h"
#include "formula.h"
#include "procedural.h"
#include "reference.h"
#include "scope.h"
#include "arithmetic.h"
#include "execution.h"

namespace gologpp {


#define DEFINE_MAKE_IMPL(GologT) \
unique_ptr<AbstractImplementation> ReadylogImplementor::make_impl(GologT &obj) \
{ return unique_ptr<AbstractImplementation>(new Implementation<GologT>(obj)); }

DEFINE_MAKE_IMPL(Action)
DEFINE_MAKE_IMPL(ExogAction)
DEFINE_MAKE_IMPL(Transition)
DEFINE_MAKE_IMPL(ExogTransition)

DEFINE_MAKE_IMPL(History)

DEFINE_MAKE_IMPL(Reference<Action>)
DEFINE_MAKE_IMPL(EffectAxiom<BooleanExpression>)
DEFINE_MAKE_IMPL(EffectAxiom<NumericExpression>)
DEFINE_MAKE_IMPL(Fluent<BooleanExpression>)
DEFINE_MAKE_IMPL(Fluent<NumericExpression>)
DEFINE_MAKE_IMPL(Reference<Fluent<BooleanExpression>>)
DEFINE_MAKE_IMPL(Reference<Fluent<NumericExpression>>)
DEFINE_MAKE_IMPL(Scope)

DEFINE_MAKE_IMPL(Constant<BooleanExpression>)
DEFINE_MAKE_IMPL(Constant<NumericExpression>)
DEFINE_MAKE_IMPL(Variable<BooleanExpression>)
DEFINE_MAKE_IMPL(Variable<NumericExpression>)
DEFINE_MAKE_IMPL(Reference<Variable<BooleanExpression>>)
DEFINE_MAKE_IMPL(Reference<Variable<NumericExpression>>)

DEFINE_MAKE_IMPL(ArithmeticOperation)

DEFINE_MAKE_IMPL(Negation)
DEFINE_MAKE_IMPL(Comparison)
DEFINE_MAKE_IMPL(Conjunction)
DEFINE_MAKE_IMPL(Disjunction)
DEFINE_MAKE_IMPL(ExistentialQuantification)
DEFINE_MAKE_IMPL(UniversalQuantification)

DEFINE_MAKE_IMPL(Block)
DEFINE_MAKE_IMPL(Choose)
DEFINE_MAKE_IMPL(Conditional)
DEFINE_MAKE_IMPL(Assignment<BooleanExpression>)
DEFINE_MAKE_IMPL(Assignment<NumericExpression>)
DEFINE_MAKE_IMPL(Pick)
DEFINE_MAKE_IMPL(Search)
DEFINE_MAKE_IMPL(Test)
DEFINE_MAKE_IMPL(While)
DEFINE_MAKE_IMPL(Procedure)
DEFINE_MAKE_IMPL(Reference<Procedure>)

DEFINE_MAKE_IMPL(Return<BooleanExpression>)
DEFINE_MAKE_IMPL(Return<NumericExpression>)

DEFINE_MAKE_IMPL(Function<BooleanExpression>)
DEFINE_MAKE_IMPL(Function<NumericExpression>)
DEFINE_MAKE_IMPL(Reference<Function<BooleanExpression>>)
DEFINE_MAKE_IMPL(Reference<Function<NumericExpression>>)

DEFINE_MAKE_IMPL(Reference<Statement>)
DEFINE_MAKE_IMPL(Reference<BooleanExpression>)
DEFINE_MAKE_IMPL(Reference<NumericExpression>)

}
