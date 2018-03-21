#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <gologpp/Implementation.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<AbstractLanguageElement> : public AbstractImplementation {
public:
	virtual ~Implementation() override = default;
	virtual EC_word term() = 0;
};


using ReadylogImplementation = Implementation<AbstractLanguageElement>;


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
	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom<ValueExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Fluent<ValueExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent<ValueExpression>> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Constant<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Constant<ValueExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Variable<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable<ValueExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(ArithmeticOperation &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Conjunction &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Disjunction &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(ExistentialQuantification &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(UniversalQuantification &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment<ValueExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Search &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Test &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(While &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Return<ValueExpression> &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Function<BooleanExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<ValueExpression> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Function<Statement> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<ValueExpression>> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Function<Statement>> &) override;
};


}

#endif
