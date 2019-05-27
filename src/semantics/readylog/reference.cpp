#include "reference.h"
#include "variable.h"

namespace gologpp {


Semantics<Reference<Variable>>::Semantics(const Reference<Variable> &ref)
: ref_(ref)
{}

EC_word Semantics<Reference<Variable>>::plterm()
{ return ref_.target()->semantics().plterm(); }



template<>
EC_word Semantics<ReferenceBase<Action, Expression>>::plterm()
{
	return ::list(::term(EC_functor("start", 2), reference_term(ref_), EC_atom("now")),
		::list(::term(EC_functor("finish", 2), reference_term(ref_), EC_atom("now")),
			::nil()
		)
	);
}

} // namespace gologpp
