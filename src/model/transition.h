#ifndef GOLOGPP_TRANSITION_H_
#define GOLOGPP_TRANSITION_H_

#include "gologpp.h"
#include "scope.h"
#include "reference.h"
#include "action.h"

namespace gologpp {



class Transition : public Grounding<Action>, public LanguageElement<Transition> {
public:
	enum Hook { START, STOP, FINISH, FAIL };

	Transition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, Hook hook);

	Transition(Transition &&) = default;
	Transition(const Transition &) = default;

	Hook hook() const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

private:
	Hook hook_;
};



string to_string(Transition::Hook);



class Activity : public Grounding<Action>, public LanguageElement<Activity> {
public:
	enum State { IDLE, RUNNING, FINAL, PREEMPTED, FAILED };

	Activity(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, State state = IDLE);
	Activity(const shared_ptr<Transition> &);

	Activity(Activity &&) = default;
	Activity(const Activity &) = default;

	State state() const;
	void set_state(State s);

	shared_ptr<Transition> transition(Transition::Hook hook);

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

	void set_sensing_result(AbstractConstant *);
	unique_ptr<AbstractConstant> &sensing_result();
	const unique_ptr<AbstractConstant> &sensing_result() const;

private:
	State state_;
	unique_ptr<AbstractConstant> sensing_result_;
};



string to_string(Activity::State s);



}

#endif // GOLOGPP_TRANSITION_H_
