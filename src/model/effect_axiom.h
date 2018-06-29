#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "action.h"
#include "reference.h"
#include "language.h"

namespace gologpp {

class Expression;
class AbstractFluent;


class AbstractEffectAxiom : public virtual AbstractLanguageElement {
public:
	AbstractEffectAxiom(const shared_ptr<AbstractAction> &action, unique_ptr<BooleanExpression> &&condition);
	AbstractEffectAxiom(AbstractEffectAxiom &&) = default;
	virtual ~AbstractEffectAxiom();

	const AbstractAction &action() const;
	const BooleanExpression &condition() const;

protected:
	shared_ptr<AbstractAction> action_;
	unique_ptr<BooleanExpression> condition_;
};


template<class ExpressionT>
class EffectAxiom : public AbstractEffectAxiom, public LanguageElement<EffectAxiom<ExpressionT>> {
public:
	EffectAxiom(const shared_ptr<AbstractAction> &action, BooleanExpression *condition,
	            Reference<Fluent<ExpressionT>> *fluent, ExpressionT *value)
	: AbstractEffectAxiom(action, unique_ptr<BooleanExpression>(condition))
	, assignment_(fluent, value, action->scope())
	{}

	EffectAxiom(AbstractAction &action, BooleanExpression *condition,
	            Reference<Fluent<ExpressionT>> *fluent, ExpressionT *value)
	: EffectAxiom(std::static_pointer_cast<AbstractAction>(action.shared_from_this()), condition, fluent, value)
	{}

	EffectAxiom(EffectAxiom<ExpressionT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const Reference<Fluent<ExpressionT>> &fluent() const
	{ return assignment_.fluent(); }
	const ExpressionT &value() const
	{ return assignment_.expression(); }

	DEFINE_IMPLEMENT_WITH_MEMBERS(*action_, *condition_, assignment_)

protected:
	Assignment<ExpressionT> assignment_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
