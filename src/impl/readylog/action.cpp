#include "action.h"
#include "effect_axiom.h"
#include "scope.h"
#include "execution.h"

#include <eclipseclass.h>

namespace gologpp {


Implementation<Action>::Implementation(const Action &a)
: action_(a)
{}


EC_word Implementation<Action>::prim_action()
{
	action_.scope().implementation().init_vars();
	return ::term(EC_functor("prim_action", 1), term());
}


EC_word Implementation<Action>::term()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		translate_args(action_.args())
	);
}


EC_word Implementation<Action>::prolog_poss_decl()
{
	action_.scope().implementation().init_vars();
	return ::term(EC_functor("prolog_poss", 1),
		term()
	);
}


EC_word Implementation<Action>::prolog_poss()
{
	EC_ref NewCondBody, S;

	action_.scope().implementation().init_vars();

	if ( ! EclipseContext::instance().ec_query(::term(
		EC_functor("trace", 1),
		::term(
			EC_functor("process_condition", 3),
			action_.precondition().implementation().term(),
			S,
			NewCondBody
		)
	)))
		throw new std::runtime_error("process_condition failed for " + action_.name());

	return ::term(EC_functor(":-", 2),
		::term(EC_functor("prolog_poss", 2),
			term(),
			S
		),
		NewCondBody
	);
}


vector<EC_word> Implementation<Action>::SSAs()
{
	action_.scope().implementation().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : action_.effects())
		rv.push_back(effect->implementation().term());
	return rv;
}


Implementation<ExogAction>::Implementation(const ExogAction &a)
: exog_(a)
{}


EC_word Implementation<ExogAction>::exog_action()
{
	exog_.scope().implementation().init_vars();
	return ::term(EC_functor("exog_action", 1), term());
}


EC_word Implementation<ExogAction>::term()
{
	return ::term(
		EC_functor(exog_.name().c_str(), exog_.arity()),
		translate_args(exog_.args())
	);
}


vector<EC_word> Implementation<ExogAction>::SSAs()
{
	exog_.scope().implementation().init_vars();
	vector<EC_word> rv;
	for (const unique_ptr<AbstractEffectAxiom> &effect : exog_.effects())
		rv.push_back(effect->implementation().term());
	return rv;
}


inline EC_word transition_term(const AbstractTransition &t) {
	EC_word *args = new EC_word[t.action().arity()];

	for (arity_t i = 0; i < t.action().arity(); i++)
		args[i] = t.args()[i]->implementation().term();

	return ::term(
		EC_functor(t.action().name().c_str(), t.action().arity()),
		args
	);
}


Implementation<Transition>::Implementation(const Transition &trans)
: trans_(trans)
{}


EC_word Implementation<Transition>::term()
{ return transition_term(trans_); }


Implementation<ExogTransition>::Implementation(const ExogTransition &trans)
: trans_(trans)
{}


EC_word Implementation<ExogTransition>::term()
{ return transition_term(trans_); }




} // namespace gologpp
