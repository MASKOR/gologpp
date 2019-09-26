#ifndef GOLOGPP_TRANSITION_H_
#define GOLOGPP_TRANSITION_H_

#include "gologpp.h"
#include "reference.h"
#include "action.h"


namespace gologpp {


class Transition : public Grounding<Action>, public LanguageElement<Transition> {
public:
	enum Hook { START, STOP, FINISH, FAIL };

	Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, Hook hook);

	Hook hook() const;
	virtual string to_string(const string &pfx) const override;
	virtual void attach_semantics(SemanticsFactory &) override;

private:
	Hook hook_;
};


string to_string(Transition::Hook);


}

#endif // GOLOGPP_TRANSITION_H_
