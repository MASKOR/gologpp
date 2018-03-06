#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

#include "gologpp.h"
#include <memory>

namespace gologpp {

class AbstractImplementation {
public:
	virtual ~AbstractImplementation() = default;
};


template<class GologT>
class Implementation;


class Implementor {
public:
	virtual ~Implementor() = default;

	virtual unique_ptr<AbstractImplementation> make_impl(Action &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Action> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<ValueExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<ValueExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<BooleanExpression>> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<ValueExpression>> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Constant<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Constant<ValueExpression> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Variable<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable<ValueExpression> &) = 0;

	//virtual unique_ptr<AbstractImplementation> make_impl(ArithmeticOperation &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Conjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Disjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(ExistentialQuantification &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(UniversalQuantification &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<ValueExpression> &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Search &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Test &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(While &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Procedure &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Procedure> &) = 0;
};


template<class ImplT, class GologT>
ImplT &impl(const GologT &obj);


}

#endif // GOLOGPP_IMPLEMENTATION_H_
