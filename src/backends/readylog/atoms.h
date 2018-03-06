#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include "Implementation.h"

#include <gologpp/Implementation.h>
#include <gologpp/atoms.h>
#include <gologpp/error.h>

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
class Implementation<Variable<ValueExpression>> : public Implementation<AbstractVariable>
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
