#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "semantics.h"
#include <model/fluent.h>

#include <eclipseclass.h>


namespace gologpp {


template<class ExprT>
class Semantics<InitialValue<ExprT>> : public ReadylogSemantics {
public:
	Semantics(const InitialValue<ExprT> &v)
	: ival_(v)
	{}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override
	{
		return ::term(
			EC_functor("initial_val", 2),
				::term(
					EC_functor(ival_.fluent().name().c_str(), ival_.fluent().arity()),
					to_ec_words(ival_.args())
				),
			ival_.value().semantics().plterm()
		);
	}

protected:
	const InitialValue<ExprT> &ival_;
};


template<>
class Semantics<AbstractFluent> : public ReadylogSemantics {
public:
	Semantics(const AbstractFluent &f);
	virtual ~Semantics() override = default;

	EC_word prim_fluent();
	virtual EC_word plterm() override;
	virtual vector<EC_word> initially() = 0;

protected:
	const AbstractFluent &fluent_;
};


template<class ExprT>
class Semantics<Fluent<ExprT>> : public Semantics<AbstractFluent> {
public:
	using Semantics<AbstractFluent>::Semantics;

	virtual vector<EC_word> initially() override
	{
		vector<EC_word> rv;
		for (const unique_ptr<InitialValue<ExprT>> &ival : static_cast<const Fluent<ExprT> &>(fluent_).initially())
			rv.push_back(ival->semantics().plterm());

		return rv;
	}
};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
