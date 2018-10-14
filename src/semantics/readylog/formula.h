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


template<class ExprT>
class Semantics<Comparison<ExprT>> : public Semantics<BooleanExpression> {
public:
	Semantics(const Comparison<ExprT> &cmp)
	: comparison_(cmp)
	{
		switch(comparison_.op()) {
		case ComparisonOperator::EQ:
			functor_ = "=";
			break;
		case ComparisonOperator::GE:
			functor_ = ">=";
			break;
		case ComparisonOperator::GT:
			functor_ = ">";
			break;
		case ComparisonOperator::LE:
			functor_ = "=<";
			break;
		case ComparisonOperator::LT:
			functor_ = "<";
			break;
		case ComparisonOperator::NEQ:
			functor_ = "\\=";
		}
	}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override
	{
		return ::term(EC_functor(functor_, 2),
			comparison_.lhs().semantics().plterm(),
			comparison_.rhs().semantics().plterm()
		);
	}

private:
	const Comparison<ExprT> &comparison_;
	const char *functor_;
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
