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
	action_.scope().impl_cast<Scope>().init_vars();
	return ::term(EC_functor("prim_action", 1), term());
}


EC_word Implementation<Action>::poss()
{
	action_.scope().impl_cast<Scope>().init_vars();
	return ::term(
		EC_functor("poss", 2), term(),
		dynamic_cast<ReadylogImplementation &>(action_.precondition().implementation()).term()
	);
}


EC_word Implementation<Action>::term()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		action_.scope().impl().variables(action_.arg_names())
	);
}


EC_word Implementation<Action>::causes_val()
{
	action_.scope().impl_cast<Scope>().init_vars();
	return dynamic_cast<ReadylogImplementation &>(action_.effect().implementation()).term();
}



} // namespace gologpp
