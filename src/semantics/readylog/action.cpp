#include "action.h"
#include "effect_axiom.h"
#include "scope.h"
#include "execution.h"
#include "variable.h"

#include <eclipseclass.h>

#include <unordered_map>

namespace gologpp {


Semantics<Action>::Semantics(const Action &a)
: action_(a)
{}


EC_word Semantics<Action>::durative_action()
{
	action_.scope().semantics().init_vars();

	vector<EC_word> arg_domains;
	for (const shared_ptr<AbstractVariable> &arg : action_.args())
		if (arg->domain().is_defined())
			arg_domains.emplace_back(
				arg->semantics<AbstractVariable>().member_restriction()
			);

	EC_word prim_action = ::term(EC_functor("durative_action", 1), plterm());

	if (arg_domains.size() > 0)
		return ::term(EC_functor(":-", 2),
			prim_action,
			to_ec_term(",", arg_domains)
		);
	else
		return prim_action;
}


EC_word Semantics<Action>::plterm()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		to_ec_words(action_.args()).data()
	);
}


vector<EC_word> Semantics<Action>::durative_causes_vals()
{
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : action_.effects())
		rv.push_back(effect->semantics().plterm());
	return rv;
}


/*vector<EC_word> Semantics<Action>::prolog_poss_decls()
{
	action_.scope().semantics().init_vars();
	return {
		::term(EC_functor("prolog_poss", 1),
			::term(EC_functor("start", 1),
				plterm())),
		::term(EC_functor("prolog_poss", 1),
			::term(EC_functor("stop", 1),
				plterm())),
		::term(EC_functor("prolog_poss", 1),
			::term(EC_functor("final", 1),
				plterm())),
		::term(EC_functor("prolog_poss", 1),
			::term(EC_functor("failed", 1),
				plterm())),
	};
}//*/


/*EC_word Semantics<Action>::get_durative_poss(const string &which)
{
	EC_ref Cond;
	EC_word q = ::term(EC_functor("poss", 2),
		::term(EC_functor(which.c_str(), 2),
			plterm(),
			::newvar()
		),
		Cond
	);
	if (!ReadylogContext::instance().ec_query(q))
		throw Bug(("failed to get durative_poss for ") + action_.str());

	return Cond;
}//*/


/*vector<EC_word> Semantics<Action>::prolog_poss()
{
	action_.scope().semantics().init_vars();

	vector<EC_word> rv;

	for (const string &evt : { "start", "stop", "final", "failed" } ) {
		EC_ref NewCondBody, S;

		if ( ! ReadylogContext::instance().ec_query(
			::term(EC_functor("process_condition", 3),
				get_durative_poss(evt),
				S,
				NewCondBody
			)
		))
			throw Bug("process_condition failed for " + action_.name());

		rv.push_back(::term(EC_functor(":-", 2),
			::term(EC_functor("prolog_poss", 2),
				plterm(),
				S
			),
			NewCondBody
		) );
	}

	return rv;
}//*/


EC_word Semantics<Action>::durative_poss()
{
	action_.scope().semantics().init_vars();
	return ::term(EC_functor("durative_poss", 2),
		plterm(),
		action_.precondition().semantics().plterm()
	);
}


/*vector<EC_word> Semantics<Action>::SSAs()
{
	action_.scope().semantics().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : action_.effects())
		rv.push_back(effect->semantics().plterm());
	return rv;
}//*/


Semantics<ExogAction>::Semantics(const ExogAction &a)
: exog_(a)
{}


EC_word Semantics<ExogAction>::exog_action()
{
	exog_.scope().semantics().init_vars();
	return ::term(EC_functor("exog_action", 1), plterm());
}


EC_word Semantics<ExogAction>::plterm()
{
	return ::term(
		EC_functor(exog_.name().c_str(), exog_.arity()),
		to_ec_words(exog_.args()).data()
	);
}


vector<EC_word> Semantics<ExogAction>::SSAs()
{
	exog_.scope().semantics().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : exog_.effects())
		rv.push_back(effect->semantics().plterm());
	return rv;
}


inline EC_word transition_term(const AbstractTransition &t) {
	EC_word *args = new EC_word[t.action()->arity()];

	for (arity_t i = 0; i < t.action()->arity(); i++)
		args[i] = t.args()[i]->semantics().plterm();

	return ::term(
		EC_functor(t.action()->name().c_str(), t.action()->arity()),
		args
	);
}


Semantics<Activity>::Semantics(const Activity &trans)
: trans_(trans)
{}


EC_word Semantics<Activity>::plterm()
{
	string state;

	switch (trans_.state()) {
	case Activity::State::IDLE:
		state = "idle";
		break;
	case Activity::State::RUNNING:
		state = "running";
		break;
	case Activity::State::FINAL:
		state = "final";
		break;
	case Activity::State::FAILED:
		state = "failed";
		break;
	case Activity::State::PREEMPTED:
		state = "preempted";
	}

	return ::term(EC_functor("exog_state_change", 3),
		transition_term(trans_),
		EC_word(ReadylogContext::instance().backend()->time().time_since_epoch().count()),
		EC_atom(state.c_str())
	);
}


Semantics<Transition>::Semantics(const Transition &trans)
: trans_(trans)
{}



EC_word Semantics<Transition>::plterm()
{
	string name;

	switch (trans_.hook()) {
	case Transition::Hook::START:
		name = "start";
		break;
	case Transition::Hook::STOP:
		name = "stop";
		break;
	case Transition::Hook::FAIL:
		name = "fail";
		break;
	case Transition::Hook::FINISH:
		name = "finish";
	}

	return ::term(EC_functor(name.c_str(), 2),
		transition_term(trans_),
		EC_word(ReadylogContext::instance().backend()->time().time_since_epoch().count())
	);
}




} // namespace gologpp
