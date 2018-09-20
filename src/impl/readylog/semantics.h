#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <model/semantics.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<AbstractLanguageElement> : public AbstractSemantics {
public:
	virtual ~Semantics() override = default;
	virtual EC_word plterm() = 0;
};


using ReadylogSemantics = Semantics<AbstractLanguageElement>;


template<class ListT>
EC_word *to_ec_words(const ListT &args)
{
	EC_word *rv = new EC_word[args.size()];
	arity_t i = 0;
	for (const auto &arg : args)
		rv[i++] = arg->semantics().plterm();
	return rv;
}


template<class ListT>
EC_word to_ec_list(const ListT &vec, typename ListT::const_iterator begin)
{
	if (begin == vec.cend())
		return ::nil();
	else
		return ::list((*begin)->semantics().plterm(), to_ec_list(vec, begin + 1));
}


class ReadylogSemanticsFactory : public SemanticsFactory {
public:
	ReadylogSemanticsFactory() = default;
	virtual ~ReadylogSemanticsFactory() override = default;

	virtual unique_ptr<AbstractSemantics> make_semantics(Action &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(ExogAction &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Transition &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(ExogTransition &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(History &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Action> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(EffectAxiom<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(EffectAxiom<NumericExpression> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(InitialValue<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(InitialValue<NumericExpression> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Fluent<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Fluent<NumericExpression> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Fluent<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Fluent<NumericExpression>> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Scope &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Constant<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Constant<NumericExpression> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Variable<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Variable<NumericExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Variable<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Variable<NumericExpression>> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(ArithmeticOperation &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Negation &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Comparison &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(BooleanOperation &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Quantification &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Block &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Choose &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Conditional &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<BooleanFluent> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<NumericFluent> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<BooleanVariable> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Assignment<NumericVariable> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Pick<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Pick<NumericExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Search &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Solve &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Test &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(While &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Return<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Return<NumericExpression> &) override;

	virtual unique_ptr<AbstractSemantics> make_semantics(Function<BooleanExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Function<NumericExpression> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Function<Statement> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<BooleanExpression>> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<NumericExpression>> &) override;
	virtual unique_ptr<AbstractSemantics> make_semantics(Reference<Function<Statement>> &) override;
};


}

#endif
