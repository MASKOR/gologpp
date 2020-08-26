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

template<>
EC_word Semantics<Negation>::plterm()
{
	return ::term(EC_functor("neg", 1),
		element().expression().semantics().plterm()
	);
}



string num_cmp_functor(const Comparison &cmp)
{
	switch(cmp.op()) {
	case ComparisonOperator::EQ:
		return "=:=";
	case ComparisonOperator::GE:
		return ">=";
	case ComparisonOperator::GT:
		return ">";
	case ComparisonOperator::LE:
		return "=<";
	case ComparisonOperator::LT:
		return "<";
	case ComparisonOperator::NEQ:
		return "=\\=";
	}
	throw Bug("Unkown comparison operator " + gologpp::to_string(cmp.op()));
}



template<>
EC_word Semantics<Comparison>::plterm()
{
	if (element().lhs().type().is<NumberType>())
		return ::term(EC_functor(num_cmp_functor(element()).c_str(), 2),
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



string bool_op_functor(const BooleanOperation &c)
{
	switch(c.op()) {
	case BooleanOperator::AND:
		return "and";
	case BooleanOperator::IFF:
		return "=";
	case BooleanOperator::IMPLIES:
		return "lif";
	case BooleanOperator::OR:
		return "or";
	case BooleanOperator::XOR:
		return "\\=";
	}
	throw Bug("Unkown BooleanOperator");
}


template<>
EC_word Semantics<BooleanOperation>::plterm()
{
	return ::term(EC_functor(bool_op_functor(element()).c_str(), 2),
		element().lhs().semantics().plterm(),
		element().rhs().semantics().plterm()
	);
}



template<>
EC_word Semantics<Quantification>::plterm()
{
	{ GologVarMutator guard(element().variable().special_semantics());

		switch (element().op()) {
		case QuantificationOperator::EXISTS:
			return ::term(EC_functor("some", 2),
				element().variable().special_semantics().plterm(),
				element().expression().semantics().plterm()
			);
		case QuantificationOperator::FORALL:
			return ::term(EC_functor("neg", 1),
				::term(EC_functor("some", 2),
					element().variable().special_semantics().plterm(),
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
