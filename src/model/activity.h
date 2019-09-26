#ifndef GOLOGPP_ACTIVITY_H_
#define GOLOGPP_ACTIVITY_H_

#include "gologpp.h"
#include "reference.h"
#include "action.h"
#include "transition.h"


namespace gologpp {


class Activity : public Grounding<Action>, public LanguageElement<Activity> {
public:
	enum State { IDLE, RUNNING, FINAL, PREEMPTED, FAILED };

	Activity(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, State state = IDLE);
	Activity(const shared_ptr<Transition> &);

	State state() const;
	void set_state(State s);

	shared_ptr<Transition> transition(Transition::Hook hook);

	const std::string &mapped_name() const;
	Value mapped_arg_value(const string &name) const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

	void set_sensing_result(Value *);
	unique_ptr<Value> &sensing_result();
	const unique_ptr<Value> &sensing_result() const;

private:
	State state_;
	unique_ptr<Value> sensing_result_;
};



string to_string(Activity::State s);



} // namespace gologpp

#endif // GOLOGPP_ACTIVITY_H_