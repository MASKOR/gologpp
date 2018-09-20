#include "action.h"
#include "effect_axiom.h"
#include "scope.h"
#include "execution.h"

#include <eclipseclass.h>

namespace gologpp {


Semantics<Action>::Semantics(const Action &a)
: action_(a)
{}


EC_word Semantics<Action>::prim_action()
{
	action_.scope().semantics().init_vars();
	return ::term(EC_functor("prim_action", 1), plterm());
}


EC_word Semantics<Action>::plterm()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		to_ec_words(action_.args())
	);
}


EC_word Semantics<Action>::prolog_poss_decl()
{
	action_.scope().semantics().init_vars();
	return ::term(EC_functor("prolog_poss", 1),
		plterm()
	);
}


EC_word Semantics<Action>::prolog_poss()
{
	EC_ref NewCondBody, S;

	action_.scope().semantics().init_vars();

	if ( ! EclipseContext::instance().ec_query(
		::term(
			EC_functor("process_condition", 3),
			action_.precondition().semantics().plterm(),
			S,
			NewCondBody
		)
	))
		throw new std::runtime_error("process_condition failed for " + action_.name());

	return ::term(EC_functor(":-", 2),
		::term(EC_functor("prolog_poss", 2),
			plterm(),
			S
		),
		NewCondBody
	);
}


vector<EC_word> Semantics<Action>::SSAs()
{
	action_.scope().semantics().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : action_.effects())
		rv.push_back(effect->semantics().plterm());
	return rv;
}


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
		to_ec_words(exog_.args())
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
	EC_word *args = new EC_word[t.action().arity()];

	for (arity_t i = 0; i < t.action().arity(); i++)
		args[i] = t.args()[i]->semantics().plterm();

	return ::term(
		EC_functor(t.action().name().c_str(), t.action().arity()),
		args
	);
}


Semantics<Transition>::Semantics(const Transition &trans)
: trans_(trans)
{}


EC_word Semantics<Transition>::plterm()
{ return transition_term(trans_); }


Semantics<ExogTransition>::Semantics(const ExogTransition &trans)
: trans_(trans)
{}


EC_word Semantics<ExogTransition>::plterm()
{ return transition_term(trans_); }




} // namespace gologpp
