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

#include <type_traits>

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


template<class LhsT>
class EffectAxiom : public AbstractEffectAxiom, public NoScopeOwner, public LanguageElement<EffectAxiom<LhsT>> {
public:
	EffectAxiom()
	{}

	EffectAxiom(EffectAxiom<LhsT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const LhsT &lhs() const
	{ return assignment_->lhs(); }

	const typename LhsT::expression_t &value() const
	{ return assignment_->rhs(); }


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + condition().to_string(pfx) + "?" + linesep
			+ pfx + indent + assignment_->to_string("") + ";";
	}

	void define(boost::optional<BooleanExpression *> condition, LhsT *lhs, typename LhsT::expression_t *value)
	{
		if (!lhs->template is_a<Reference<AbstractFluent>>()) {
			AbstractFieldAccess *fa = dynamic_cast<AbstractFieldAccess *>(lhs);
			if (!fa || !fa->subject().is_a<CompoundFluent>())
				throw std::runtime_error("Effect must assign to a fluent or to a compound fluent's field");
		}

		if (condition)
			set_condition(*condition);

		assignment_ = std::make_unique<Assignment<LhsT>>(lhs, value);
		assignment_->set_parent(this);
	}

	virtual Scope &parent_scope() override
	{ return action().scope(); }

	virtual const Scope &parent_scope() const override
	{ return action().scope(); }


	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *assignment_)

protected:
	unique_ptr<Assignment<LhsT>> assignment_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
