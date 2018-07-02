#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "implementation.h"
#include <model/gologpp.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<BooleanExpression> : public ReadylogImplementation {
};


template<>
class Implementation<Negation> : public Implementation<BooleanExpression> {
public:
	Implementation(const Negation &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const Negation &negation_;
};


template<>
class Implementation<Comparison> : public Implementation<BooleanExpression> {
public:
	Implementation(const Comparison &);
	virtual ~Implementation() override;

	virtual EC_word term() override;

private:
	const Comparison &comparison_;
	const char *functor_;
};


template<>
class Implementation<BooleanOperation> : public Implementation<BooleanExpression> {
public:
	Implementation(const BooleanOperation &);
	virtual EC_word term() override;

private:
	const BooleanOperation &conjunction_;
	const char *functor_;
};


template<>
class Implementation<Quantification> : public Implementation<BooleanExpression> {
public:
	Implementation(const Quantification &);
	virtual EC_word term() override;

private:
	const Quantification &quantification_;
};



} // namespace gologpp


#endif
