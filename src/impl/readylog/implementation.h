#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <model/implementation.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<AbstractLanguageElement> : public AbstractImplementation {
public:
	virtual ~Implementation() override = default;
	virtual EC_word term() = 0;
};


using ReadylogImplementation = Implementation<AbstractLanguageElement>;


template<class ListT>
EC_word *translate_args(const ListT &args)
{
	EC_word *rv = new EC_word[args.size()];
	arity_t i = 0;
	for (const auto &arg : args)
		rv[i++] = arg->implementation().term();
	return rv;
}


class ReadylogImplementor : public Implementor {
public:
	ReadylogImplementor() = default;
	virtual ~ReadylogImplementor() override = default;

	virtual unique_ptr<AbstractImplementation> make_impl(Action &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(ExogAction &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Transition &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(ExogTransition &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(History &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Action> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<NumericExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(InitialValue<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(InitialValue<NumericExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<NumericExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<NumericExpression>> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Constant<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Constant<NumericExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Variable<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable<NumericExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Variable<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Variable<NumericExpression>> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(ArithmeticOperation &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Comparison &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(BooleanOperation &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Quantification &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanFluent> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<NumericFluent> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanVariable> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<NumericVariable> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Search &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Solve &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Test &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(While &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<NumericExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Function<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<NumericExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<Statement> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<NumericExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<Statement>> &) override;
};


}

#endif
