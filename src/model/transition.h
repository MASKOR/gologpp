#ifndef GOLOGPP_TRANSITION_H_
#define GOLOGPP_TRANSITION_H_

#include "gologpp.h"
#include "scope.h"

namespace gologpp {


class AbstractTransition
: public NoScopeOwner
, public virtual AbstractLanguageElement {
public:
	AbstractTransition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args);
	AbstractTransition(const AbstractTransition &);

	AbstractTransition &operator = (const AbstractTransition &);

	bool operator == (const AbstractTransition &) const;
	bool operator != (const AbstractTransition &) const;

	shared_ptr<Action> action() const;
	const vector<unique_ptr<AbstractConstant>> &args() const;
	virtual string to_string(const string &pfx) const override;
	size_t hash() const;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	struct Hash {
		std::size_t operator () (const shared_ptr<AbstractTransition> &t) const
		{ return t->hash(); }
	};

	struct Equals {
		bool operator () (
			const shared_ptr<AbstractTransition> &lhs,
			const shared_ptr<AbstractTransition> &rhs
		) const
		{ return *lhs == *rhs; }
	};

protected:
	shared_ptr<Action> action_;
	vector<unique_ptr<AbstractConstant>> args_; // TODO: class ArgList
};



class Transition : public AbstractTransition, public LanguageElement<Transition> {
public:
	enum Hook { START, STOP, FINISH, FAIL };

	Transition(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, Hook hook);
	Transition &operator = (const Transition &);

	Transition(Transition &&) = default;
	Transition(const Transition &) = default;

	Hook hook() const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

private:
	Hook hook_;
};



string to_string(Transition::Hook);



class Activity : public AbstractTransition, public LanguageElement<Activity> {
public:
	enum State { IDLE, RUNNING, FINAL, PREEMPTED, FAILED };

	Activity(const shared_ptr<Action> &action, vector<unique_ptr<AbstractConstant>> &&args, State state = IDLE);
	Activity(const shared_ptr<Transition> &);
	Activity &operator = (const Activity &);

	Activity(Activity &&) = default;
	Activity(const Activity &) = default;

	State state() const;
	void set_state(State s);

	shared_ptr<Transition> transition(Transition::Hook hook) const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

private:
	State state_;
};



string to_string(Activity::State s);



}

#endif // GOLOGPP_TRANSITION_H_
