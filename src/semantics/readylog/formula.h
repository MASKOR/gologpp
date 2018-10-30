#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "semantics.h"
#include <model/gologpp.h>
#include <model/formula.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<Negation> : public ReadylogSemantics {
public:
	Semantics(const Negation &);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

private:
	const Negation &negation_;
};


template<>
class Semantics<Comparison<NumericExpression>> : public ReadylogSemantics {
public:
	Semantics(const Comparison<NumericExpression> &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison<NumericExpression> &comparison_;
	const char *functor_;
};


template<>
class Semantics<Comparison<SymbolicExpression>> : public ReadylogSemantics {
public:
	Semantics(const Comparison<SymbolicExpression> &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison<SymbolicExpression> &comparison_;
};


template<>
class Semantics<Comparison<StringExpression>> : public ReadylogSemantics {
public:
	Semantics(const Comparison<StringExpression> &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison<StringExpression> &comparison_;
};


template<>
class Semantics<BooleanOperation> : public ReadylogSemantics {
public:
	Semantics(const BooleanOperation &);
	virtual EC_word plterm() override;

private:
	const BooleanOperation &conjunction_;
	const char *functor_;
};


template<>
class Semantics<Quantification> : public ReadylogSemantics {
public:
	Semantics(const Quantification &);
	virtual EC_word plterm() override;

private:
	const Quantification &quantification_;
};



} // namespace gologpp


#endif
