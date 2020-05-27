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

#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include "semantics.h"
#include "reference.h"

#include <model/action.h>
#include <execution/activity.h>
#include <model/reference.h>

namespace gologpp {

class Action;


template<>
class Semantics<Action>
: public Semantics<ModelElement>
, public GeneralSemantics<Action>
{
public:
	using GeneralSemantics<Action>::GeneralSemantics;
};


template<>
class Semantics<ExogAction>
: public Semantics<ModelElement>
, public GeneralSemantics<ExogAction>
{
public:
	using GeneralSemantics<ExogAction>::GeneralSemantics;
};


template<>
class Semantics<Reference<ExogAction>>
: public Semantics<Instruction>
, public GeneralSemantics<Reference<ExogAction>>
{
public:
	using GeneralSemantics<Reference<ExogAction>>::GeneralSemantics;
};


template<>
class Semantics<Activity>
: public Semantics<ModelElement>
, public GeneralSemantics<Activity>
{
public:
	using GeneralSemantics<Activity>::GeneralSemantics;
};



} // namespace gologpp


#endif
