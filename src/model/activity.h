#ifndef GOLOGPP_ACTIVITY_H_
#define GOLOGPP_ACTIVITY_H_

#include "gologpp.h"
#include "reference.h"
#include "action.h"
#include "transition.h"

#include <boost/optional.hpp>


namespace gologpp {


class Activity
: public Grounding<Action>
, public LanguageElement<Activity>
, public std::enable_shared_from_this<Activity> {
public:
	enum State { IDLE, RUNNING, FINAL, PREEMPTED, FAILED };

	Activity(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, AExecutionContext &, State state = IDLE);
	Activity(const shared_ptr<Transition> &, AExecutionContext &);

	State state() const;
	void set_state(State s);

	void update(Transition::Hook hook, boost::optional<Value> &&sensing_result = {});

	const std::string &mapped_name() const;
	Value mapped_arg_value(const string &name) const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

	void set_sensing_result(boost::optional<Value> &&);
	boost::optional<Value> &sensing_result();
	const boost::optional<Value> &sensing_result() const;

	static State target_state(Transition::Hook);

private:
	State state_;
	boost::optional<Value> sensing_result_;
	AExecutionContext &exec_context_;
};



string to_string(Activity::State s);



} // namespace gologpp

#endif // GOLOGPP_ACTIVITY_H_
