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
	typedef VoidExpression expression_t;

	AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);

	AbstractAction(const AbstractAction &) = delete;
	AbstractAction(AbstractAction &&other) = default;

	virtual ~AbstractAction() override = default;

	virtual bool operator == (const AbstractAction &other) const = 0;
	bool operator != (const AbstractAction &other) const;


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

	bool operator == (const AbstractAction &other) const override;

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

private:
	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<ActionMapping> mapping_;
};



class ExogAction : public AbstractAction, public LanguageElement<ExogAction> {
public:
	using AbstractAction::AbstractAction;
	ExogAction(const Action &) = delete;
	ExogAction(ExogAction &&) = default;
	bool operator == (const AbstractAction &other) const override;
	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
};




} // namespace gologpp




#endif /* GOLOGPP_ACTION_H_ */
