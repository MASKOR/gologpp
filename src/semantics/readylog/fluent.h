#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "semantics.h"
#include "utilities.h"
#include "scope.h"
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

		const Fluent<ExprT> &f = dynamic_cast<const Fluent<ExprT> &>(fluent_);


		vector<EC_word> domain_tuples;
		for (const vector<unique_ptr<AbstractConstant>> &domain_tp : f.domain())
			domain_tuples.emplace_back(to_ec_term(",", domain_tp));

		return ::term(EC_functor(":-", 2),
			::term(EC_functor("prim_fluent", 1),
				plterm()
			),
			::term(EC_functor("member", 2),
				to_ec_term(",", f.args()),
				to_ec_list(domain_tuples)
			)
		);
	}

};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
