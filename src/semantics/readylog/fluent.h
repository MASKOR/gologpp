#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "semantics.h"
#include "utilities.h"
#include "scope.h"
#include "variable.h"
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

	virtual EC_word prim_fluent() = 0;
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

	virtual EC_word prim_fluent() override
	{
		fluent_.scope().semantics().init_vars();

		vector<EC_word> arg_domains;
		for (const shared_ptr<AbstractVariable> &arg : fluent_.args())
			if (arg->domain().is_defined())
				arg_domains.emplace_back(
					arg->semantics<AbstractVariable>().member_restriction()
				);

		EC_word prim_fluent = ::term(EC_functor("prim_fluent", 1), plterm());

		if (arg_domains.size() > 0)
			return ::term(EC_functor(":-", 2),
				prim_fluent,
				to_ec_term(",", arg_domains)
			);
		else
			return prim_fluent;
	}

};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
