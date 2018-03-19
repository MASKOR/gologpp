#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <memory>
#include <algorithm>

#include "gologpp.h"

#include "error.h"
#include "Formula.h"
#include "atoms.h"
#include "Language.h"
#include "Scope.h"

namespace gologpp {


class AbstractEffectAxiom;


class AbstractAction : public Statement, public Identifier, public virtual AbstractLanguageElement {
public:
	AbstractAction(const string &name, const vector<string> &arg_names,
	       unique_ptr<BooleanExpression> &&precondition = nullptr, unique_ptr<AbstractEffectAxiom> &&effect = nullptr);

	AbstractAction(const Action &) = delete;
	AbstractAction(AbstractAction &&other) = default;

	virtual ~AbstractAction() override = default;

	const BooleanExpression &precondition() const;

	template<class T>
	void set_precondition(T &&precondition)
	{ precondition_ = unique_ptr<BooleanExpression>(new T(std::move(precondition))); }

	const AbstractEffectAxiom &effect() const;

	template<class T>
	void set_effect(T &&effect)
	{ effect_ = unique_ptr<AbstractEffectAxiom>(new T(std::move(effect))); }

	template<class ExpressionT>
	void declare_variable(const string &name)
	{
		if (std::find(args_.begin(), args_.end(), name) == args_.end())
			throw Bug("Variable `" + name + "' not defined for Action `" + this->name() + "'");
		scope_.variable<ExpressionT>(name);
	}

	const vector<string> &arg_names() const;

	shared_ptr<AbstractVariable> argument(arity_t idx) const;

	Scope &scope();
	const Scope &scope() const;

	virtual void implement(Implementor &) override;

protected:
	Scope scope_;

	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<AbstractEffectAxiom> effect_;
	vector<string> args_;
};


class Action : public AbstractAction, public LanguageElement<Action> {
public:
	using AbstractAction::AbstractAction;
};


class ExogAction : public AbstractAction, public LanguageElement<ExogAction> {
public:
	using AbstractAction::AbstractAction;
};



} // namespace gologpp



#endif /* GOLOGPP_ACTION_H_ */
