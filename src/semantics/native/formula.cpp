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
#include <model/reference.h>

namespace gologpp {

template<>
Value Semantics<Negation>::evaluate(const Binding &b, const History &h)
{ return !element().expression().semantics<Expression>().evaluate(b, h); }



template<>
Value gologpp::Semantics<Comparison>::evaluate(const Binding &b, const History &h)
{
	switch(element().op()) {
	case Comparison::Operator::EQ:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				== element().lhs().semantics<Expression>().evaluate(b, h)
		);
	case Comparison::Operator::NEQ:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				!= element().lhs().semantics<Expression>().evaluate(b, h)
		);
	case Comparison::Operator::GE:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				>= element().lhs().semantics<Expression>().evaluate(b, h)
		);
	case Comparison::Operator::LE:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				<= element().lhs().semantics<Expression>().evaluate(b, h)
		);
	case Comparison::Operator::GT:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				> element().lhs().semantics<Expression>().evaluate(b, h)
		);
	case Comparison::Operator::LT:
		return Value(
			get_type<BoolType>(),
			element().lhs().semantics<Expression>().evaluate(b, h)
				< element().lhs().semantics<Expression>().evaluate(b, h)
		);
	}
}



template<>
Value gologpp::Semantics<BooleanOperation>::evaluate(const Binding &b, const History &h)
{
	bool lhs = static_cast<bool>(element().lhs().semantics<Expression>().evaluate(b, h));
	bool rhs = static_cast<bool>(element().rhs().semantics<Expression>().evaluate(b, h));

	switch (element().op()) {
	case BooleanOperator::OR:
		return Value(get_type<BoolType>(), lhs || rhs);
	case BooleanOperator::AND:
		return Value(get_type<BoolType>(), lhs && rhs);
	case BooleanOperator::IFF:
		return Value(get_type<BoolType>(), lhs == rhs);
	case BooleanOperator::XOR:
		return Value(get_type<BoolType>(), lhs != rhs);
	case BooleanOperator::IMPLIES:
		return Value(get_type<BoolType>(), !lhs || rhs);
	}
}



template<>
Value gologpp::Semantics<Quantification>::evaluate(const Binding &b, const History &h) {
	switch (element().op()) {
	case Quantification::Operator::FORALL:
		for (const auto &val : element().variable().domain().elements()) {
			// TODO: What if domain empty?
			Binding b1(b);
			b1.bind(element().variable().shared(), *val);
			if (!static_cast<bool>(element().expression().semantics<Expression>().evaluate(b1, h)))
				return Value(get_type<BoolType>(), false);
		}
		return Value(get_type<BoolType>(), true);
	case Quantification::Operator::EXISTS:
		for (const auto &val : element().variable().domain().elements()) {
			Binding b1(b);
			b1.bind(element().variable().shared(), *val);
			if (static_cast<bool>(element().expression().semantics<Expression>().evaluate(b1, h)))
				return Value(get_type<BoolType>(), true);
		}
		return Value(get_type<BoolType>(), false);
	}
}



}
