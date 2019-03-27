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
#include "reference.h"

namespace gologpp {


class AbstractEffectAxiom;


class AbstractAction
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement {
public:
	typedef VoidExpression expression_t;
	typedef AbstractAction abstract_t;
	typedef Void type_t;

	AbstractAction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);
	AbstractAction(Scope &parent_scope, const string &name);

	virtual ~AbstractAction() override = default;

	virtual bool operator == (const AbstractAction &other) const;
	bool operator != (const AbstractAction &other) const;

	const BooleanExpression &precondition() const;
	BooleanExpression &precondition();

	void set_precondition(BooleanExpression *);

	const vector<unique_ptr<AbstractEffectAxiom>> &effects() const;
	vector<unique_ptr<AbstractEffectAxiom>> &effects();
	void add_effect(AbstractEffectAxiom *effect);

	virtual void compile(AExecutionContext &ctx) override;
	virtual string to_string(const string &pfx) const override;

	const ActionMapping &mapping() const;
	ActionMapping &mapping();
	void set_mapping (ActionMapping *);

	virtual void attach_semantics(SemanticsFactory &) override;

private:
	vector<unique_ptr<AbstractEffectAxiom>> effects_;
	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<ActionMapping> mapping_;
};



class Action : public AbstractAction, public LanguageElement<Action> {
public:
	using AbstractAction::AbstractAction;

	void set_senses(Reference<AbstractFluent> *);

	void define(
		boost::optional<BooleanExpression *> precondition,
		boost::optional<vector<AbstractEffectAxiom *>> effects,
		boost::optional<Reference<AbstractFluent> *> senses
	);

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Expression *ref(const vector<Expression *> &args) override;
	Reference<Action> *make_ref(const vector<Expression *> &args);

	unique_ptr<Reference<AbstractFluent>> &senses();
	const unique_ptr<Reference<AbstractFluent>> &senses() const;

private:
	unique_ptr<Reference<AbstractFluent>> senses_;
};



class ExogAction : public AbstractAction, public LanguageElement<ExogAction> {
public:
	using AbstractAction::AbstractAction;

	void define(
		boost::optional<BooleanExpression *> precondition,
		boost::optional<vector<AbstractEffectAxiom *>> effects
	);

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Expression *ref(const vector<Expression *> &args) override;
	Reference<ExogAction> *make_ref(const vector<Expression *> &args);
};



class ExogEvent : public Grounding<ExogAction>, public LanguageElement<ExogEvent> {
public:
	using Grounding<ExogAction>::Grounding;

	virtual void attach_semantics(SemanticsFactory &) override;
};

} // namespace gologpp




#endif /* GOLOGPP_ACTION_H_ */
