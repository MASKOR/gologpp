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

#ifndef BACKENDS_READYLOG_SCOPE_H_
#define BACKENDS_READYLOG_SCOPE_H_

#include <model/semantics.h>
#include <model/scope.h>

#include <eclipseclass.h>

#include "semantics.h"

namespace gologpp {


template<>
class Semantics<Scope>
: public GeneralSemantics<Scope>
{
public:
	Semantics(const Scope &, ReadylogContext &context);

	EC_word plterm();

	EC_word *variables(const vector<string> &names);
};


} // namespace gologpp

#endif // BACKENDS_READYLOG_SCOPE_H_
