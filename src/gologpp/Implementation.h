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

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Action &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Action> &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::EffectAxiom &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Fluent &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Fluent> &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Scope &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::AnyValue &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Variable &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Negation &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Conjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Disjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::ExistentialQuantification &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::UniversalQuantification &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Block &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Choose &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Conditional &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Assignment &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Pick &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Search &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Test &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::While &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(generic::Procedure &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(generic::Reference<generic::Procedure> &) = 0;
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
