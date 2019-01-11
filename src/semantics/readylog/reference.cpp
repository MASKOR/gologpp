#include "reference.h"

namespace gologpp {

template<>
EC_word Semantics<Reference<Action>>::plterm()
{
	return ::list(::term(EC_functor("start", 2), reference_term(reference_), EC_atom("now")),
		::list(::term(EC_functor("finish", 2), reference_term(reference_), EC_atom("now")),
			::nil()
		)
	);
}

} // namespace gologpp