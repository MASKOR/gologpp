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

#ifndef READYLOG_COMPOUND_EXPRESSION_H_
#define READYLOG_COMPOUND_EXPRESSION_H_

#include "semantics.h"

#include <model/compound_expression.h>

namespace gologpp {


template<>
class Semantics<CompoundExpression>
: public Semantics<Expression>
, public GeneralSemantics<CompoundExpression>
{
public:
	using GeneralSemantics<CompoundExpression>::GeneralSemantics;
	virtual Value evaluate(const AbstractReference &context, const History &h) override;
};


}

#endif // READYLOG_COMPOUND_EXPRESSION_H_
