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

#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include "semantics.h"
#include "scope.h"
#include "variable.h"
#include "utilities.h"
#include "fluent.h"
#include "reference.h"

#include <model/semantics.h>
#include <model/procedural.h>

namespace gologpp {




template<>
class Semantics<Choose>
: public Semantics<Instruction>
, public GeneralSemantics<Choose>
{
public:
	using GeneralSemantics<Choose>::GeneralSemantics;
	virtual unique_ptr<Plan> trans(const Binding &b, History &h) override;
	virtual unique_ptr<Plan> plan(const Binding &b, History &h) override;

private:
	vector<unique_ptr<Instruction>>::const_iterator ip;
};



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
