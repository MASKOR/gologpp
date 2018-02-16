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

Implementation<generic::Comparison>::Implementation(const generic::Comparison &c)
: comparison_(c)
{}


EC_word Implementation<generic::Comparison>::term()
{
}


Implementation<generic::Conjunction>::Implementation(const generic::Conjunction &c)
: conjunction_(c)
{}


EC_word Implementation<generic::Conjunction>::term()
{
}


Implementation<generic::Disjunction>::Implementation(const generic::Disjunction &d)
: disjunction_(d)
{}


EC_word Implementation<generic::Disjunction>::term()
{
}


Implementation<generic::ExistentialQuantification>::Implementation(const generic::ExistentialQuantification &q)
: quantification_(q)
{}


EC_word Implementation<generic::ExistentialQuantification>::term()
{
}


Implementation<generic::UniversalQuantification>::Implementation(const generic::UniversalQuantification &q)
: quantification_(q)
{}


EC_word Implementation<generic::UniversalQuantification>::term()
{
}




}
