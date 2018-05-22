#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "implementation.h"
#include <eclipseclass.h>


namespace gologpp {


template<>
class Implementation<AbstractFluent> : public ReadylogImplementation {
public:
	Implementation(const AbstractFluent &f);
	virtual ~Implementation() override = default;

	EC_word prim_fluent();
	virtual EC_word term() override;

private:
	const AbstractFluent &fluent_;
};


template<class ExprT>
class Implementation<Fluent<ExprT>> : public Implementation<AbstractFluent> {
public:
	using Implementation<AbstractFluent>::Implementation;
};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
