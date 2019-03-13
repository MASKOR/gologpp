#include "string.h"

namespace gologpp {


Semantics<ToString>::Semantics(const ToString &to_string)
: to_string_(to_string)
{}


EC_word Semantics<ToString>::plterm()
{
	return ::term(EC_functor("to_string", 1),
		to_string_.expression().semantics().plterm()
	);
}



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
