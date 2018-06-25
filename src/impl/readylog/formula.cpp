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


Implementation<Comparison>::~Implementation()
{}


Implementation<Comparison>::Implementation(const Comparison &cmp)
: comparison_(cmp)
{
	switch(comparison_.cmp_operator()) {
	case ComparisonOperator::eq:
		functor_ = "=:=";
		break;
	case ComparisonOperator::ge:
		functor_ = ">=";
		break;
	case ComparisonOperator::gt:
		functor_ = ">";
		break;
	case ComparisonOperator::le:
		functor_ = "=<";
		break;
	case ComparisonOperator::lt:
		functor_ = "<";
		break;
	case ComparisonOperator::neq:
		functor_ = "=\\=";
	}
}


EC_word Implementation<Comparison>::term()
{
	return ::term(EC_functor(functor_, 2),
		comparison_.lhs().implementation().term(),
		comparison_.rhs().implementation().term()
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
