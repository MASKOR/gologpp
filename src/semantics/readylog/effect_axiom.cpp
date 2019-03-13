#include "effect_axiom.h"


namespace gologpp {


Semantics<AbstractEffectAxiom>::Semantics(const AbstractEffectAxiom &eff)
: effect_(eff)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
	try {
		dynamic_cast<const Action &>(effect_.action());
		// All golog++ actions are implicitly durative
		cv_functor = "durative_causes_val";
	} catch (std::bad_cast &) {
		try {
			dynamic_cast<const ExogAction &>(effect_.action());
			cv_functor = "causes_val";
		} catch (std::bad_cast &) {
			throw Bug("Unknown action type");
		}
	}
#pragma GCC diagnostic pop
}


}