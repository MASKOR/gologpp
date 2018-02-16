#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <memory>

#include "gologpp.h"

#include "EffectAxiom.h"
#include "error.h"
#include "Formula.h"
#include "atoms.h"
#include "Language.h"
#include "Scope.h"

namespace gologpp {
namespace generic {


class Action : public enable_shared_from_this<Action>, public Identifier, public LanguageElement<Action> {
public:
	Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition = nullptr, unique_ptr<EffectAxiom> &&effect = nullptr);

	Action(const Action &) = delete;
	Action(Action &&other) = default;

	virtual ~Action() override = default;

	const BooleanExpression &precondition() const;

	template<class T>
	void set_precondition(T &&precondition)
	{ precondition_ = unique_ptr<BooleanExpression>(new T(std::move(precondition))); }

	const EffectAxiom &effect() const;
	void set_effect(EffectAxiom &&effect);

	const vector<string> &args() const;

	Scope &scope();
	const Scope &scope() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*precondition_, *effect_, scope_)

protected:
	Scope scope_;

	unique_ptr<BooleanExpression> precondition_;
	unique_ptr<EffectAxiom> effect_;
	vector<string> args_;
};



class Transition {
public:
	Transition(const shared_ptr<Action> &action, vector<unique_ptr<AnyValue>> &&binding);

	const Action &action() const;

private:
	const shared_ptr<Action> action_;
	const vector<unique_ptr<AnyValue>> binding_;
};


} // namespace generic
} // namespace gologpp



#endif /* GOLOGPP_ACTION_H_ */
