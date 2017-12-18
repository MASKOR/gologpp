#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <initializer_list>
#include <memory>

#include "gologpp.h"

#include "PrecondAxiom.h"
#include "EffectAxiom.h"
#include "error.h"
#include "Formula.h"
#include "Execution.h"

namespace gologpp {

using namespace std;


class Action : public enable_shared_from_this<Action>, public LanguageElement, public NameWithArity<Action> {
public:
	Action(const string &name, arity_t arity, unique_ptr<PrecondAxiom> &&precondition, unique_ptr<EffectAxiom> &&effect);

	virtual ~Action() = default;

	const PrecondAxiom &precondition() const;
	const EffectAxiom &effect() const;

private:
	unique_ptr<PrecondAxiom> precondition_;
	unique_ptr<EffectAxiom> effect_;
};


class Transition {
public:
	Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&binding);

	const Action &action() const;

private:
	const shared_ptr<Action> action_;
	const vector<unique_ptr<Value>> binding_;
};


} /* namespace gologpp */



#endif /* GOLOGPP_ACTION_H_ */
