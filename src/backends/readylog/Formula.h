#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "Implementation.h"
#include <gologpp/gologpp.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<BooleanExpression> : public ReadylogExpression {
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
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const Comparison &comparison_;
};


template<>
class Implementation<Conjunction> : public Implementation<BooleanExpression> {
public:
	Implementation(const Conjunction &);
	virtual EC_word term() override;

private:
	const Conjunction &conjunction_;
};


template<>
class Implementation<Disjunction> : public Implementation<BooleanExpression> {
public:
	Implementation(const Disjunction &);
	virtual EC_word term() override;

private:
	const Disjunction &disjunction_;
};


template<>
class Implementation<ExistentialQuantification> : public Implementation<BooleanExpression> {
public:
	Implementation(const ExistentialQuantification &);
	virtual EC_word term() override;

private:
	const ExistentialQuantification &quantification_;
};


template<>
class Implementation<UniversalQuantification> : public Implementation<BooleanExpression> {
public:
	Implementation(const UniversalQuantification &);
	virtual EC_word term() override;

private:
	const UniversalQuantification &quantification_;
};


} // namespace gologpp


#endif
