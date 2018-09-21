#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/atoms.h>
#include <model/error.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<AbstractVariable> : public ReadylogSemantics {
public:
	Semantics(const AbstractVariable &var);
	virtual ~Semantics() override;

	void init();
	virtual EC_word plterm() override;
	void translate_as_golog_var(bool as_gv);

private:
	const AbstractVariable &var_;
	EC_word ec_var_;
	EC_atom golog_var_;
	bool as_golog_var_;
};


template<>
class Semantics<Variable<BooleanExpression>> : public Semantics<AbstractVariable>
{
public:
	using Semantics<AbstractVariable>::Semantics;
};


template<>
class Semantics<Variable<NumericExpression>> : public Semantics<AbstractVariable>
{
public:
	using Semantics<AbstractVariable>::Semantics;
};



/**
 * Scope guard that changes a variable implementation to render itself as a
 * Golog variable instead of a Prolog variable
 */
class GologVarMutator {
public:
	GologVarMutator(Semantics<AbstractVariable> &var_impl);

	template<class ExpressionT>
	GologVarMutator(const Reference<Variable<ExpressionT>> &var_ref)
	: GologVarMutator(var_ref.target()->template implementation<AbstractVariable>())
	{}

	~GologVarMutator();
private:
	Semantics<AbstractVariable> &var_impl_;
};



template<class ExpressionT>
class Semantics<Constant<ExpressionT>> : public ReadylogSemantics {
public:
	Semantics(const Constant<ExpressionT> &value)
	: value_(value)
	{}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

private:
	const Constant<ExpressionT> &value_;
};


} // namespace gologpp



#endif
