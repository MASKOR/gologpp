#include "Formula.h"
#include <gologpp/Formula.h>

namespace gologpp {

Implementation<Negation>::Implementation(const Negation &neg)
: negation_(neg)
{}


EC_word Implementation<Negation>::term()
{
	return ::term(EC_functor("neg", 1),
	              dynamic_cast<ReadylogExpression &>(
	                  negation_.expression().implementation()
	              ).term()
	);
}

Implementation<Comparison>::Implementation(const Comparison &c)
: comparison_(c)
{}


EC_word Implementation<Comparison>::term()
{
}


Implementation<Conjunction>::Implementation(const Conjunction &c)
: conjunction_(c)
{}


EC_word Implementation<Conjunction>::term()
{
}


Implementation<Disjunction>::Implementation(const Disjunction &d)
: disjunction_(d)
{}


EC_word Implementation<Disjunction>::term()
{
}


Implementation<ExistentialQuantification>::Implementation(const ExistentialQuantification &q)
: quantification_(q)
{}


EC_word Implementation<ExistentialQuantification>::term()
{
}


Implementation<UniversalQuantification>::Implementation(const UniversalQuantification &q)
: quantification_(q)
{}


EC_word Implementation<UniversalQuantification>::term()
{
}




}
