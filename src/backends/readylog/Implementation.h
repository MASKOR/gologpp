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

	virtual unique_ptr<AbstractImplementation> get_impl(Action &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Action> &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(EffectAxiom &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Fluent &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Fluent> &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Scope &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(AnyValue &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Variable &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(Negation &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Conjunction &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Disjunction &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(ExistentialQuantification &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(UniversalQuantification &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(Block &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Choose &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Conditional &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Assignment &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Pick &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Search &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Test &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(While &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(Procedure &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Procedure> &) override;
};



}

#endif
