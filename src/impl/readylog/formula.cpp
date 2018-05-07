#include "formula.h"
#include <model/formula.h>
#include <model/atoms.h>

namespace gologpp {

Implementation<Negation>::Implementation(const Negation &neg)
: negation_(neg)
{}


EC_word Implementation<Negation>::term()
{
	return ::term(EC_functor("neg", 1),
		dynamic_cast<ReadylogImplementation &>(
			negation_.expression().implementation()
		).term()
	);
}


Implementation<Conjunction>::Implementation(const Conjunction &c)
: conjunction_(c)
{}


EC_word Implementation<Conjunction>::term()
{
	return ::term(EC_functor("and", 2),
		conjunction_.lhs().implementation().term(),
		conjunction_.rhs().implementation().term()
	);
}


Implementation<Disjunction>::Implementation(const Disjunction &d)
: disjunction_(d)
{}


EC_word Implementation<Disjunction>::term()
{
	return ::term(EC_functor("or", 2),
		disjunction_.lhs().implementation().term(),
		disjunction_.rhs().implementation().term()
	);
}


Implementation<ExistentialQuantification>::Implementation(const ExistentialQuantification &q)
: quantification_(q)
{}


EC_word Implementation<ExistentialQuantification>::term()
{
	return ::term(EC_functor("some", 2),
		quantification_.variable().implementation().term(),
		quantification_.expression().implementation().term()
	);
}


Implementation<UniversalQuantification>::Implementation(const UniversalQuantification &q)
: quantification_(q)
{}


EC_word Implementation<UniversalQuantification>::term()
{
	return ::term(EC_functor("neg", 1),
		::term(EC_functor("some", 2),
			quantification_.variable().implementation().term(),
			::term(EC_functor("neg", 1),
				quantification_.expression().implementation().term()
			)
		)
	);
}




}
