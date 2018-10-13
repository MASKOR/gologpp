#include "formula.h"
#include "variable.h"

#include <model/formula.h>
#include <model/error.h>

namespace gologpp {

Semantics<Negation>::Semantics(const Negation &neg)
: negation_(neg)
{}


EC_word Semantics<Negation>::plterm()
{
	return ::term(EC_functor("neg", 1),
		dynamic_cast<ReadylogSemantics &>(
			negation_.expression().semantics()
		).plterm()
	);
}


Semantics<Comparison>::~Semantics()
{}


Semantics<Comparison>::Semantics(const Comparison &cmp)
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


EC_word Semantics<Comparison>::plterm()
{
	return ::term(EC_functor(functor_, 2),
		comparison_.lhs().semantics().plterm(),
		comparison_.rhs().semantics().plterm()
	);
}


Semantics<BooleanOperation>::Semantics(const BooleanOperation &c)
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


EC_word Semantics<BooleanOperation>::plterm()
{
	return ::term(EC_functor(functor_, 2),
		conjunction_.lhs().semantics().plterm(),
		conjunction_.rhs().semantics().plterm()
	);
}


Semantics<Quantification>::Semantics(const Quantification &q)
: quantification_(q)
{}


EC_word Semantics<Quantification>::plterm()
{
	{ GologVarMutator guard(quantification_.variable().semantics<AbstractVariable>());

		switch (quantification_.op()) {
		case QuantificationOperator::EXISTS:
			return ::term(EC_functor("some", 2),
				quantification_.variable().semantics().plterm(),
				quantification_.expression().semantics().plterm()
			);
		case QuantificationOperator::FORALL:
			return ::term(EC_functor("neg", 1),
				::term(EC_functor("some", 2),
					quantification_.variable().semantics().plterm(),
					::term(EC_functor("neg", 1),
						quantification_.expression().semantics().plterm()
					)
				)
			);
		}
	}
	throw Bug("Unknown QuantificationOperator");
}




}
