#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <gologpp/Implementation.h>

#include <eclipseclass.h>

namespace gologpp {



class ReadylogImplementation : public AbstractImplementation {
public:
	virtual ~ReadylogImplementation() override = default;
	virtual EC_word term() = 0;
};


class ReadylogImplementor : public Implementor {
public:
	ReadylogImplementor() = default;
	virtual ~ReadylogImplementor() override = default;

	virtual unique_ptr<AbstractImplementation> make_impl(Action &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Action> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Fluent &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent> &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(AnyValue &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Conjunction &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Disjunction &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(ExistentialQuantification &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(UniversalQuantification &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Pick &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Search &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Test &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(While &) override;

	virtual unique_ptr<AbstractImplementation> make_impl(Procedure &) override;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Procedure> &) override;
};


}

#endif
