#include "string_concatenation.h"

namespace gologpp {

Semantics<StringConcatenation>::Semantics(const StringConcatenation &subject)
: subject_(subject)
{}


EC_word Semantics<StringConcatenation>::plterm()
{
	return ::term(EC_functor("strcat", 2),
		subject_.lhs().semantics().plterm(),
		subject_.rhs().semantics().plterm()
	);
}


}
