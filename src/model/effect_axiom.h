#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "action.h"
#include "reference.h"
#include "language.h"
#include "expressions.h"
#include "fluent.h"
#include "procedural.h"

#include <boost/optional.hpp>

namespace gologpp {

class Expression;
class AbstractFluent;


class AbstractEffectAxiom : public virtual AbstractLanguageElement {
public:
	AbstractEffectAxiom();
	AbstractEffectAxiom(AbstractEffectAxiom &&) = default;
	virtual ~AbstractEffectAxiom();

	const AbstractAction &action() const;
	AbstractAction &action();
	void set_action(AbstractAction &action);
	const BooleanExpression &condition() const;
	BooleanExpression &condition();
	void set_condition(BooleanExpression *condition);

protected:
	AbstractAction *action_;
	unique_ptr<BooleanExpression> condition_;
};


template<class ExpressionT>
class EffectAxiom : public AbstractEffectAxiom, public NoScopeOwner, public LanguageElement<EffectAxiom<ExpressionT>> {
public:
	EffectAxiom()
	{}

	EffectAxiom(EffectAxiom<ExpressionT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const Reference<Fluent<ExpressionT>> &fluent() const
	{ return assignment_->lhs(); }

	const ExpressionT &value() const
	{ return assignment_->rhs(); }


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + condition().to_string(pfx) + "?" + linesep
			+ pfx + indent + assignment_->to_string("") + ";";
	}

	void define(boost::optional<BooleanExpression *> condition, Reference<Fluent<ExpressionT>> *fluent, ExpressionT *value)
	{
		if (condition)
			set_condition(*condition);

		assignment_ = std::make_unique<Assignment<Fluent<ExpressionT>>>(fluent, value);
		assignment_->set_parent(this);
	}

	virtual Scope &parent_scope() override
	{ return action().scope(); }

	virtual const Scope &parent_scope() const override
	{ return action().scope(); }


	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *assignment_)

protected:
	unique_ptr<Assignment<Fluent<ExpressionT>>> assignment_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
