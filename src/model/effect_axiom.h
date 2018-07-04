#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "action.h"
#include "reference.h"
#include "language.h"

#include <boost/optional.hpp>

namespace gologpp {

class Expression;
class AbstractFluent;


class AbstractEffectAxiom : public virtual AbstractLanguageElement {
public:
	AbstractEffectAxiom(const AbstractAction *action, BooleanExpression *condition);
	AbstractEffectAxiom(AbstractEffectAxiom &&) = default;
	virtual ~AbstractEffectAxiom();

	const AbstractAction &action() const;
	const BooleanExpression &condition() const;

protected:
	const AbstractAction *action_;
	unique_ptr<BooleanExpression> condition_;
};


template<class ExpressionT>
class EffectAxiom : public AbstractEffectAxiom, public LanguageElement<EffectAxiom<ExpressionT>> {
public:
	EffectAxiom(AbstractAction *action, BooleanExpression *condition,
	            Reference<Fluent<ExpressionT>> *fluent, ExpressionT *value)
	: AbstractEffectAxiom(action, condition)
	, assignment_(fluent, value, action->scope())
	{}

	EffectAxiom(
		AbstractAction *action,
		boost::optional<BooleanExpression *> condition,
	    Reference<Fluent<ExpressionT>> *fluent,
		ExpressionT *value
	)
	: EffectAxiom(
		action,
		condition.get_value_or(new BooleanConstant(true)),
		fluent,
		value
	)
	{}

	EffectAxiom(EffectAxiom<ExpressionT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const Reference<Fluent<ExpressionT>> &fluent() const
	{ return assignment_.lhs(); }
	const ExpressionT &value() const
	{ return assignment_.rhs(); }

	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, assignment_)

protected:
	Assignment<Fluent<ExpressionT>> assignment_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
