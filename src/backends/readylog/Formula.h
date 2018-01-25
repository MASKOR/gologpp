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

} // namespace gologpp


#endif