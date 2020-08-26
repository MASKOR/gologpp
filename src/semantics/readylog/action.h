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

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<AbstractAction>
: public ReadylogSemantics {
};


template<>
class Semantics<Action>
: public Semantics<AbstractAction>
, public GeneralSemantics<Action>
{
public:
	using GeneralSemantics<Action>::GeneralSemantics;
	virtual ~Semantics() override = default;

	EC_word durative_action();
	vector<EC_word> durative_causes_vals();
	EC_word durative_poss();
	EC_word senses();

	virtual EC_word plterm() override;
};


template<>
class Semantics<ExogAction>
: public Semantics<AbstractAction>
, public GeneralSemantics<ExogAction>
{
public:
	using GeneralSemantics<ExogAction>::GeneralSemantics;
	virtual ~Semantics() override = default;

	EC_word exog_action();
	vector<EC_word> causes_vals();
	EC_word poss();

	virtual EC_word plterm() override;
};



} // namespace gologpp


#endif
