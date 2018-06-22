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
	Implementation(const AbstractVariable &);
	virtual ~Implementation() override;

	void init();
	virtual EC_word term() override;

private:
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
