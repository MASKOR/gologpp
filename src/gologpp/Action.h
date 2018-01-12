#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <set>
#include <memory>

#include "gologpp.h"

#include "PrecondAxiom.h"
#include "EffectAxiom.h"
#include "error.h"
#include "Formula.h"
#include "Translation.h"
#include "atoms.h"

namespace gologpp {

using namespace std;


class Action : public enable_shared_from_this<Action>, public NameWithArity, public Scope, public Expression {
public:
	Action(const string &name, const vector<string> &args,
	       unique_ptr<Expression> &&precondition, unique_ptr<EffectAxiom> &&effect);
	Action(Action &&other);

	virtual ~Action() = default;

	const Expression &precondition() const;
	const EffectAxiom &effect() const;

protected:
	unique_ptr<Expression> precondition_;
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
