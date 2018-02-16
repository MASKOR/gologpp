#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "Implementation.h"

#include <eclipseclass.h>

namespace gologpp {

namespace generic {
class BooleanExpression;
class Negation;
class Comparison;
class ConnectiveFormula;
class ExistentialQuantification;
class UniversalQuantification;
} // namespace generic


template<>
class Implementation<generic::BooleanExpression> : public ReadylogExpression {
};


template<>
class Implementation<generic::Negation> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::Negation &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const generic::Negation &negation_;
};


template<>
class Implementation<generic::Comparison> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::Comparison &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const generic::Comparison &comparison_;
};


template<>
class Implementation<generic::Conjunction> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::Conjunction &);
	virtual EC_word term() override;

private:
	const generic::Conjunction &conjunction_;
};


template<>
class Implementation<generic::Disjunction> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::Disjunction &);
	virtual EC_word term() override;

private:
	const generic::Disjunction &disjunction_;
};


template<>
class Implementation<generic::ExistentialQuantification> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::ExistentialQuantification &);
	virtual EC_word term() override;

private:
	const generic::ExistentialQuantification &quantification_;
};


template<>
class Implementation<generic::UniversalQuantification> : public Implementation<generic::BooleanExpression> {
public:
	Implementation(const generic::UniversalQuantification &);
	virtual EC_word term() override;

private:
	const generic::UniversalQuantification &quantification_;
};


} // namespace gologpp


#endif
