#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "Action.h"
#include "Reference.h"
#include "Language.h"

namespace gologpp {

class Expression;
class AbstractFluent;


class AbstractEffectAxiom : public virtual AbstractLanguageElement {
public:
	AbstractEffectAxiom(Reference<Action> &&action);
	AbstractEffectAxiom(AbstractEffectAxiom &&) = default;
	virtual ~AbstractEffectAxiom();

	const Reference<Action> &action() const;

protected:
	Reference<Action> action_;
};


template<class ExpressionT>
class EffectAxiom : public AbstractEffectAxiom, public LanguageElement<EffectAxiom<ExpressionT>> {
public:
	EffectAxiom(Reference<Action> &&action, Reference<Fluent<ExpressionT>> &&fluent, unique_ptr<ExpressionT> &&value)
	: AbstractEffectAxiom(std::move(action))
	, fluent_(std::move(fluent))
	, value_(std::move(value))
	{}

	EffectAxiom(EffectAxiom<ExpressionT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const Reference<Fluent<ExpressionT>> &fluent() const
	{ return fluent_; }
	const ExpressionT &value() const
	{ return *value_; }

	DEFINE_IMPLEMENT_WITH_MEMBERS(action_, fluent_, *value_)

protected:
	Reference<Fluent<ExpressionT>> fluent_;
	unique_ptr<ExpressionT> value_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
