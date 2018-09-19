#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

#include "gologpp.h"
#include <memory>

namespace gologpp {


template<class GologT>
class Implementation;


class AbstractImplementation {
public:
	virtual ~AbstractImplementation() = default;
};


class Implementor {
public:
	virtual ~Implementor() = default;

	virtual unique_ptr<AbstractImplementation> make_impl(Action &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(ExogAction &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Transition &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(ExogTransition &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Action> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(InitialValue<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(InitialValue<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<NumericExpression>> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Constant<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Constant<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Variable<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Variable<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Variable<NumericExpression>> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(ArithmeticOperation &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Comparison &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(BooleanOperation &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Quantification &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanFluent> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<NumericFluent> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanVariable> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<NumericVariable> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Search &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Solve &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Test &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(While &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<NumericExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Function<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<NumericExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<Statement> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<NumericExpression>> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<Statement>> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(History &) = 0;
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
