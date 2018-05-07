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
	Implementation(const AbstractVariable &var)
	: variable_(var)
	{}

	virtual ~Implementation() override = default;

	void init()
	{ ec_var_ = ::newvar(); }

	virtual EC_word term() override
	{ return ec_var_; }

private:
	const AbstractVariable &variable_;
	EC_word ec_var_;
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
