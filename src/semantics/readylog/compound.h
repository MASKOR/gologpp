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

#include <model/compound.h>

namespace gologpp {


std::pair<const Reference<Fluent> *, EC_word>
traverse_mixed_field_access(const FieldAccess *fa, const ListAccess *la);


template<>
class Semantics<FieldAccess>
: public Semantics<Expression>
, public GeneralSemantics<FieldAccess>
{
public:
	using GeneralSemantics<FieldAccess>::GeneralSemantics;

	virtual EC_word plterm() override;
	EC_word field_assign(const Expression &value);
	EC_atom pl_field_name();
};



}

#endif // READYLOG_COMPOUND_EXPRESSION_H_
