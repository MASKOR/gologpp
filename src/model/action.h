#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <memory>
#include <algorithm>

#include <boost/optional.hpp>

#include "gologpp.h"

#include "language.h"
#include "scope.h"
#include "global.h"
#include "mapping.h"

namespace gologpp {


class AbstractEffectAxiom;


class AbstractAction
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement {
public:
	typedef Statement expression_t;

	AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);

	AbstractAction(const AbstractAction &) = delete;
	AbstractAction(AbstractAction &&other) = default;

	virtual ~AbstractAction() override = default;

	const vector<unique_ptr<AbstractEffectAxiom>> &effects() const;
	void add_effect(AbstractEffectAxiom *effect);

	virtual void compile(AExecutionContext &ctx) override;
	virtual string to_string(const string &pfx) const override;

protected:
	vector<unique_ptr<AbstractEffectAxiom>> effects_;
};



class Action : public AbstractAction, public LanguageElement<Action> {
public:
	typedef AbstractAction abstract_t;

	Action(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);
	Action(Scope &parent_scope, const string &name);

	Action(const Action &) = delete;
	Action(Action &&) = default;

	const BooleanExpression &precondition() const;

	void set_precondition(BooleanExpression *);

	void define(
		boost::optional<BooleanExpression *> precondition,
		boost::optional<vector<AbstractEffectAxiom *>> effects
	);

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Expression *ref(const vector<Expression *> &args) override;
	Reference<Action> *make_ref(const vector<Expression *> &args);

	const ActionMapping &mapping() const;
	void set_mapping (ActionMapping *);

	bool blocking() const;

	unique_ptr<Reference<Procedure>> &on_succeed();
	unique_ptr<Reference<Procedure>> &on_preempted();
	unique_ptr<Reference<Procedure>> &on_failed();

private:
	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<ActionMapping> mapping_;

	unique_ptr<Reference<Procedure>> on_succeed_;
	unique_ptr<Reference<Procedure>> on_preempted_;
	unique_ptr<Reference<Procedure>> on_failed_;
};



class ExogAction : public AbstractAction, public LanguageElement<ExogAction> {
public:
	using AbstractAction::AbstractAction;
	ExogAction(const Action &) = delete;
	ExogAction(ExogAction &&) = default;
	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
};



class AbstractTransition
: public NoScopeOwner
, public virtual AbstractLanguageElement {
public:
	AbstractTransition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args);
	AbstractTransition(const AbstractTransition &);

	AbstractTransition &operator = (const AbstractTransition &);

	const Action &action() const;
	const vector<unique_ptr<AbstractConstant>> &args() const;
	virtual string to_string(const string &pfx) const override;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

protected:
	shared_ptr<Action> action_;
	vector<unique_ptr<AbstractConstant>> args_;
};



class Transition : public AbstractTransition, public LanguageElement<Transition> {
public:
	using AbstractTransition::AbstractTransition;
	using AbstractTransition::operator =;

	Transition(Transition &&) = default;
	Transition(const Transition &);

	virtual void attach_semantics(SemanticsFactory &) override;
};



class ExogTransition : public AbstractTransition, public LanguageElement<ExogTransition> {
public:
	using AbstractTransition::AbstractTransition;
	using AbstractTransition::operator =;

	ExogTransition(ExogTransition &&) = default;
	ExogTransition(const ExogTransition &);

	virtual void attach_semantics(SemanticsFactory &) override;
};



} // namespace gologpp



#endif /* GOLOGPP_ACTION_H_ */
