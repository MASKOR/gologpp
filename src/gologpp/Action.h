#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <memory>

#include "gologpp.h"

#include "error.h"
#include "Formula.h"
#include "atoms.h"
#include "Language.h"
#include "Scope.h"

namespace gologpp {


class AbstractEffectAxiom;


class Action : public enable_shared_from_this<Action>, public Identifier, public LanguageElement<Action> {
public:
	typedef Statement expression_t;

	Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition = nullptr, unique_ptr<AbstractEffectAxiom> &&effect = nullptr);

	Action(const Action &) = delete;
	Action(Action &&other) = default;

	virtual ~Action() override = default;

	const BooleanExpression &precondition() const;

	template<class T>
	void set_precondition(T &&precondition)
	{ precondition_ = unique_ptr<BooleanExpression>(new T(std::move(precondition))); }

	const AbstractEffectAxiom &effect() const;

	template<class T>
	void set_effect(T &&effect)
	{ effect_ = unique_ptr<AbstractEffectAxiom>(new T(std::move(effect))); }

	const vector<string> &args() const;

	Scope &scope();
	const Scope &scope() const;

	virtual void implement(Implementor &) override;

protected:
	Scope scope_;

	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<AbstractEffectAxiom> effect_;
	vector<string> args_;
};



} // namespace gologpp



#endif /* GOLOGPP_ACTION_H_ */
