#include "Formula.h"
#include <gologpp/Formula.h>

namespace gologpp {

Implementation<generic::Negation>::Implementation(const generic::Negation &neg)
: negation_(neg)
{}


EC_word Implementation<generic::Negation>::term()
{
	return ::term(EC_functor("neg", 1),
	              dynamic_cast<ReadylogExpression &>(
	                  negation_.expression().implementation()
	              ).term()
	);
}


}