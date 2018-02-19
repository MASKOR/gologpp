#include "Action.h"
#include "Execution.h"
#include "EffectAxiom.h"
#include "Scope.h"
#include <gologpp/Action.h>

#include <eclipseclass.h>
#include <utility>

namespace gologpp {

Implementation<generic::Action>::Implementation(const generic::Action &a)
: action_(a)
{}


EC_word Implementation<generic::Action>::prim_action()
{
	action_.scope().impl_cast<generic::Scope>().init_vars();
	return ::term(EC_functor("prim_action", 1), term());
}


EC_word Implementation<generic::Action>::poss()
{
	action_.scope().impl_cast<generic::Scope>().init_vars();
	return ::term(
		EC_functor("poss", 2), term(),
		dynamic_cast<ReadylogExpression &>(action_.precondition().implementation()).term()
	);
}


EC_word Implementation<generic::Action>::term()
{
	return ::term(
		EC_functor(action_.name().c_str(), action_.arity()),
		action_.scope().impl_cast<generic::Scope>().variables(action_.args())
	);
}


EC_word Implementation<generic::Action>::causes_val()
{
	action_.scope().impl_cast<generic::Scope>().init_vars();
	return action_.effect().impl_cast<generic::EffectAxiom>().term();
}



} // namespace gologpp
