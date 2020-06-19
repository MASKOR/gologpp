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

#ifndef READYLOG_ARITHMETIC_H_
#define READYLOG_ARITHMETIC_H_


#include <model/arithmetic.h>
#include "semantics.h"
#include <eclipseclass.h>

namespace gologpp {

template<>
class Semantics<ArithmeticOperation>
: public Semantics<Expression>
, public GeneralSemantics<ArithmeticOperation>
{
public:
	Semantics(const ArithmeticOperation &, ReadylogContext &context);

	EC_word plterm() override;

private:
	const char *functor_;
};


}

#endif // READYLOG_ARITHMETIC_H_
