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

	virtual unique_ptr<AbstractImplementation> make_impl(EffectAxiom &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Fluent &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Reference<Fluent> &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Scope &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(AnyValue &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Variable &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Negation &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Conjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Disjunction &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(ExistentialQuantification &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(UniversalQuantification &) = 0;

	virtual unique_ptr<AbstractImplementation> make_impl(Block &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Choose &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Conditional &) = 0;
	virtual unique_ptr<AbstractImplementation> make_impl(Assignment &) = 0;
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
