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

#ifndef READYLOG_STRING_CONCATENATION_H_
#define READYLOG_STRING_CONCATENATION_H_

#include "semantics.h"

#include <model/string.h>

namespace gologpp {


template<>
class Semantics<ToString>
: public Semantics<Expression>
, public GeneralSemantics<ToString>
{
public:
	using GeneralSemantics<ToString>::GeneralSemantics;
	virtual Value evaluate(const History &h) override;
};


template<>
class Semantics<StringConcatenation>
: public Semantics<Expression>
, public GeneralSemantics<StringConcatenation>
{
public:
	using GeneralSemantics<StringConcatenation>::GeneralSemantics;
	virtual Value evaluate(const History &h) override;
};


}

#endif // READYLOG_STRING_CONCATENATION_H_
