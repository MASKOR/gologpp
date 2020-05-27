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

#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include <model/reference.h>

#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <model/utilities.h>

#include "semantics.h"
#include "utilities.h"
#include "variable.h"


namespace gologpp {

template<class GologT> class Reference;

class NativeContext;

template<>
class Semantics<Reference<Variable>>
: public Semantics<Expression>
, public GeneralSemantics<Reference<Variable>>
{
public:
	using GeneralSemantics<Reference<Variable>>::GeneralSemantics;
	virtual Value evaluate(const Binding &b, const History &h) override;
};



template<>
class Semantics<Reference<Function>>
: public GeneralSemantics<Reference<Function>>
, public Semantics<Expression>
{
public:
	using GeneralSemantics<Reference<Function>>::GeneralSemantics;
	virtual Value evaluate(const Binding &b, const History &h) override;
};



template<>
class Semantics<Reference<Procedure>>
: public GeneralSemantics<Reference<Procedure>>
, public Semantics<Instruction>
{
public:
	using GeneralSemantics<Reference<Procedure>>::GeneralSemantics;
	unique_ptr<Plan> trans(const Binding &b, History &h) override;
};



template<>
class Semantics<Reference<Action>>
: public GeneralSemantics<Reference<Action>>
, public Semantics<Instruction>
{
public:
	using GeneralSemantics<Reference<Action>>::GeneralSemantics;
	unique_ptr<Plan> trans(const Binding &b, History &h) override;
	unique_ptr<Plan> plan(const Binding &b, History &h) override;
};



template<>
class Semantics<Reference<Procedure>>;

template<>
class Semantics<Reference<Function>>;

template<>
class Semantics<Reference<Fluent>>;



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
