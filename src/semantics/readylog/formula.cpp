/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "formula.h"
#include "variable.h"

#include <model/formula.h>
#include <model/error.h>

namespace gologpp {

Semantics<Negation>::Semantics(const Negation &neg)
: AbstractSemantics<Negation>(neg)
{}


EC_word Semantics<Negation>::plterm()
{
	return ::term(EC_functor("neg", 1),
		element().expression().semantics().plterm()
	);
}



Semantics<Comparison>::Semantics(const Comparison &cmp)
: AbstractSemantics<Comparison>(cmp)
{
	switch(element().op()) {
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
	if (element().lhs().type().is<NumberType>())
		return ::term(EC_functor(functor_, 2),
			element().lhs().semantics().plterm(),
			element().rhs().semantics().plterm()
		);
	else {
		EC_word lhs = element().lhs().semantics().plterm();
		EC_word rhs = element().rhs().semantics().plterm();

		switch(element().op()) {
		case ComparisonOperator::EQ:
			return ::term(EC_functor("=", 2), lhs, rhs);
		case ComparisonOperator::GE:
			return ::term(EC_functor(";", 2),
				::term(EC_functor("compare", 3), EC_atom(">"), lhs, rhs),
				::term(EC_functor("=", 2), lhs, rhs)
			);
		case ComparisonOperator::GT:
			return ::term(EC_functor("compare", 3), EC_atom(">"), lhs, rhs);
		case ComparisonOperator::LE:
			return ::term(EC_functor(";", 2),
				::term(EC_functor("compare", 3), EC_atom("<"), lhs, rhs),
				::term(EC_functor("=", 2), lhs, rhs)
			);
		case ComparisonOperator::LT:
			return ::term(EC_functor("compare", 3), EC_atom("<"), lhs, rhs);
		case ComparisonOperator::NEQ:
			return ::term(EC_functor("\\=", 2), lhs, rhs);
		}
		throw Bug("Unknown comparison operator");
	}
}



Semantics<BooleanOperation>::Semantics(const BooleanOperation &c)
: AbstractSemantics<BooleanOperation>(c)
{
	switch(element().op()) {
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
		element().lhs().semantics().plterm(),
		element().rhs().semantics().plterm()
	);
}


Semantics<Quantification>::Semantics(const Quantification &q)
: AbstractSemantics<Quantification>(q)
{}


EC_word Semantics<Quantification>::plterm()
{
	{ GologVarMutator guard(element().variable().semantics());

		switch (element().op()) {
		case QuantificationOperator::EXISTS:
			return ::term(EC_functor("some", 2),
				element().variable().semantics().plterm(),
				element().expression().semantics().plterm()
			);
		case QuantificationOperator::FORALL:
			return ::term(EC_functor("neg", 1),
				::term(EC_functor("some", 2),
					element().variable().semantics().plterm(),
					::term(EC_functor("neg", 1),
						element().expression().semantics().plterm()
					)
				)
			);
		}
	}
	throw Bug("Unknown QuantificationOperator");
}



}
