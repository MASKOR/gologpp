#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

#include "gologpp.h"
#include <memory>

namespace gologpp {


template<class GologT>
class Semantics;


class AbstractSemantics {
public:
	virtual ~AbstractSemantics() = default;
};


class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;

	virtual unique_ptr<AbstractSemantics> make_semantics(Action &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(ExogAction &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Transition &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(ExogTransition &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Action> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(EffectAxiom<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(EffectAxiom<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(InitialValue<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(InitialValue<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Fluent<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Fluent<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Fluent<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Fluent<NumericExpression>> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Scope &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Constant<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Constant<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Variable<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Variable<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Variable<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Variable<NumericExpression>> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(ArithmeticOperation &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Negation &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Comparison &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(BooleanOperation &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Quantification &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Block &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Choose &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Conditional &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<BooleanFluent> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<NumericFluent> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<BooleanVariable> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<NumericVariable> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Pick<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Pick<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Search &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Solve &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Test &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(While &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Return<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Return<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(Function<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Function<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Function<Statement> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<NumericExpression>> &) = 0;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<Statement>> &) = 0;

	virtual unique_ptr<AbstractSemantics> make_semantics(History &) = 0;
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
