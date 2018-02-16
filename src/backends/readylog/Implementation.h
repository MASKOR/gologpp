#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <gologpp/Implementation.h>

#include <eclipseclass.h>

namespace gologpp {



class ReadylogExpression : public AbstractImplementation {
public:
	virtual ~ReadylogExpression() override = default;
	virtual EC_word term() = 0;
};


class ReadylogImplementor : public Implementor {
public:
	ReadylogImplementor() = default;
	virtual ~ReadylogImplementor() override = default;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Action &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Action> &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::EffectAxiom &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Fluent &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Fluent> &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Scope &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::AnyValue &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Variable &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Negation &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Conjunction &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Disjunction &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::ExistentialQuantification &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::UniversalQuantification &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Block &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Choose &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Conditional &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Assignment &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Pick &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Search &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Test &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::While &) override;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Procedure &) override;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Procedure> &) override;
};



}

#endif
