#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include "implementation.h"

#include <model/implementation.h>
#include <model/atoms.h>
#include <model/error.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<AbstractVariable> : public ReadylogImplementation {
public:
	Implementation(const AbstractVariable &var);
	virtual ~Implementation() override;

	void init();
	virtual EC_word term() override;
	void translate_as_golog_var(bool as_gv);

private:
	const AbstractVariable &var_;
	EC_word ec_var_;
	EC_word golog_var_;
	bool as_golog_var_;
};


template<>
class Implementation<Variable<BooleanExpression>> : public Implementation<AbstractVariable>
{
public:
	using Implementation<AbstractVariable>::Implementation;
};


template<>
class Implementation<Variable<NumericExpression>> : public Implementation<AbstractVariable>
{
public:
	using Implementation<AbstractVariable>::Implementation;
};



/**
 * Scope guard that changes a variable implementation to render itself as a
 * Golog variable instead of a Prolog variable
 */
class GologVarMutator {
public:
	GologVarMutator(Implementation<AbstractVariable> &var_impl);

	template<class ExpressionT>
	GologVarMutator(const Reference<Variable<ExpressionT>> &var_ref)
	: GologVarMutator(var_ref.target()->template implementation<AbstractVariable>())
	{}

	~GologVarMutator();
private:
	Implementation<AbstractVariable> &var_impl_;
};



template<class ExpressionT>
class Implementation<Constant<ExpressionT>> : public ReadylogImplementation {
public:
	Implementation(const Constant<ExpressionT> &value)
	: value_(value)
	{}

	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const Constant<ExpressionT> &value_;
};


} // namespace gologpp



#endif
