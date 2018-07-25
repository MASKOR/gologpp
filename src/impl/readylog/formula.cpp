#include "formula.h"
#include "atoms.h"
#include <model/formula.h>
#include <model/atoms.h>
#include <model/error.h>

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
	switch(comparison_.op()) {
	case ComparisonOperator::EQ:
		functor_ = "=";
		break;
	case ComparisonOperator::GE:
		functor_ = ">=";
		break;
	case ComparisonOperator::GT:
		functor_ = ">";
		break;
	case ComparisonOperator::LE:
		functor_ = "=<";
		break;
	case ComparisonOperator::LT:
		functor_ = "<";
		break;
	case ComparisonOperator::NEQ:
		functor_ = "\\=";
	}
}


EC_word Implementation<Comparison>::term()
{
	return ::term(EC_functor(functor_, 2),
		comparison_.lhs().implementation().term(),
		comparison_.rhs().implementation().term()
	);
}


Implementation<BooleanOperation>::Implementation(const BooleanOperation &c)
: conjunction_(c)
{
	switch(c.op()) {
	case BooleanOperator::AND:
		functor_ = "and";
		break;
	case BooleanOperator::IFF:
		functor_ = "=";
		break;
	case BooleanOperator::IMPLIES:
		functor_ = "lif";
		break;
	case BooleanOperator::OR:
		functor_ = "or";
		break;
	case BooleanOperator::XOR:
		functor_ = "\\=";
		break;
	}
}


EC_word Implementation<BooleanOperation>::term()
{
	return ::term(EC_functor(functor_, 2),
		conjunction_.lhs().implementation().term(),
		conjunction_.rhs().implementation().term()
	);
}


Implementation<Quantification>::Implementation(const Quantification &q)
: quantification_(q)
{}


EC_word Implementation<Quantification>::term()
{
	{ GologVarMutator guard(quantification_.variable().implementation<AbstractVariable>());

		switch (quantification_.op()) {
		case QuantificationOperator::EXISTS:
			return ::term(EC_functor("some", 2),
				quantification_.variable().implementation().term(),
				quantification_.expression().implementation().term()
			);
		case QuantificationOperator::FORALL:
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
	throw Bug("Unknown QuantificationOperator");
}




}
