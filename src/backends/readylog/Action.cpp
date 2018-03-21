#include "Action.h"
#include "EffectAxiom.h"
#include "Scope.h"

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


EC_word Implementation<Action>::poss()
{
	action_.scope().implementation().init_vars();
	return ::term(
		EC_functor("poss", 2), term(),
		action_.precondition().implementation().term()
	);
}


EC_word Implementation<Action>::term()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		action_.scope().implementation().variables(action_.arg_names())
	);
}


EC_word Implementation<Action>::causes_val()
{
	action_.scope().implementation().init_vars();
	return action_.effect().implementation().term();
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
		exog_.scope().implementation().variables(exog_.arg_names())
	);
}


EC_word Implementation<ExogAction>::causes_val()
{
	exog_.scope().implementation().init_vars();
	return exog_.effect().implementation().term();
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
