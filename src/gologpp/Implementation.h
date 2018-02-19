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

	virtual unique_ptr<AbstractImplementation> get_impl(Action &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Action> &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(EffectAxiom &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(Fluent &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Fluent> &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(Scope &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(AnyValue &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Variable &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(Negation &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Conjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Disjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(ExistentialQuantification &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(UniversalQuantification &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(Block &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Choose &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Conditional &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Assignment &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Pick &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Search &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Test &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(While &) = 0;

	virtual unique_ptr<AbstractImplementation> get_impl(Procedure &) = 0;
	virtual unique_ptr<AbstractImplementation> get_impl(Reference<Procedure> &) = 0;
};


}

#endif // GOLOGPP_IMPLEMENTATION_H_
