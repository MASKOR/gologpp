#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "semantics.h"
#include <model/gologpp.h>
#include <model/formula.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<BooleanExpression> : public ReadylogSemantics {
};


template<>
class Semantics<Negation> : public Semantics<BooleanExpression> {
public:
	Semantics(const Negation &);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

private:
	const Negation &negation_;
};


template<>
class Semantics<Comparison<NumericExpression>> : public Semantics<BooleanExpression> {
public:
	Semantics(const Comparison<NumericExpression> &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison<NumericExpression> &comparison_;
	const char *functor_;
};


template<>
class Semantics<Comparison<SymbolicExpression>> : public Semantics<BooleanExpression> {
public:
	Semantics(const Comparison<SymbolicExpression> &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison<SymbolicExpression> &comparison_;
};


template<>
class Semantics<BooleanOperation> : public Semantics<BooleanExpression> {
public:
	Semantics(const BooleanOperation &);
	virtual EC_word plterm() override;

private:
	const BooleanOperation &conjunction_;
	const char *functor_;
};


template<>
class Semantics<Quantification> : public Semantics<BooleanExpression> {
public:
	Semantics(const Quantification &);
	virtual EC_word plterm() override;

private:
	const Quantification &quantification_;
};



} // namespace gologpp


#endif
