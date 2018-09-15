#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "implementation.h"
#include <model/fluent.h>

#include <eclipseclass.h>


namespace gologpp {


template<class ExprT>
class Implementation<InitialValue<ExprT>> : public ReadylogImplementation {
public:
	Implementation(const InitialValue<ExprT> &v)
	: ival_(v)
	{}

	virtual ~Implementation() override = default;

	virtual EC_word term() override
	{
		return ::term(
			EC_functor("initial_val", 2),
				::term(
					EC_functor(ival_.fluent().name().c_str(), ival_.fluent().arity()),
					translate_args(ival_.args())
				),
			ival_.value().implementation().term()
		);
	}

protected:
	const InitialValue<ExprT> &ival_;
};


template<>
class Implementation<AbstractFluent> : public ReadylogImplementation {
public:
	Implementation(const AbstractFluent &f);
	virtual ~Implementation() override = default;

	EC_word prim_fluent();
	virtual EC_word term() override;
	virtual vector<EC_word> initially() = 0;

protected:
	const AbstractFluent &fluent_;
};


template<class ExprT>
class Implementation<Fluent<ExprT>> : public Implementation<AbstractFluent> {
public:
	using Implementation<AbstractFluent>::Implementation;

	virtual vector<EC_word> initially() override
	{
		vector<EC_word> rv;
		for (const unique_ptr<InitialValue<ExprT>> &ival : static_cast<const Fluent<ExprT> &>(fluent_).initially())
			rv.push_back(ival->implementation().term());

		return rv;
	}
};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
