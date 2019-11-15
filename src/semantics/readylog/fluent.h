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

#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "semantics.h"
#include <model/fluent.h>

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<InitialValue>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<InitialValue>
{
public:
	Semantics(const InitialValue &v);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Fluent>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Fluent>
{
public:
	using AbstractSemantics<Fluent>::AbstractSemantics;
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
	vector<EC_word> initially();
	EC_word prim_fluent();
};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
