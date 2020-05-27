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

#ifndef READYLOG_VARIABLE_H_
#define READYLOG_VARIABLE_H_

#include "semantics.h"

#include <model/variable.h>
#include <model/semantics.h>


namespace gologpp {


template<>
class Semantics<Variable>
: public Semantics<Expression>
, public GeneralSemantics<Variable>
{
public:
	using GeneralSemantics<Variable>::GeneralSemantics;
	virtual Value evaluate(const Binding &b, const History &h) override;
};




} // namespace gologpp

#endif // READYLOG_VARIABLE_H_
