#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <memory>

#include "gologpp.h"

#include "EffectAxiom.h"
#include "error.h"
#include "Formula.h"
#include "Translation.h"
#include "atoms.h"
#include "Language.h"

namespace gologpp {
namespace generic {

using namespace std;


class Action : public enable_shared_from_this<Action>, public NameWithArity, public LanguageElement<Action> {
public:
	Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition = nullptr, unique_ptr<EffectAxiom> &&effect = nullptr);
	Action(Action &&other);

	virtual ~Action() = default;

	const BooleanExpression &precondition() const;

	template<class T>
	void set_precondition(T &&precondition)
	{ precondition_ = std::move(make_unique<BooleanExpression>(std::move(precondition))); }

	const EffectAxiom &effect() const;
	void set_effect(unique_ptr<EffectAxiom> &&effect);
	vector<shared_ptr<Variable>> args();

	Scope &scope();

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
