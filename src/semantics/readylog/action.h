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
#include <model/activity.h>

#include <eclipseclass.h>

namespace gologpp {

class Action;


template<>
class Semantics<Action>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Action>
{
public:
	using AbstractSemantics<Action>::AbstractSemantics;
	virtual ~Semantics() override = default;

	EC_word durative_action();
	vector<EC_word> durative_causes_vals();
	EC_word durative_poss();
	EC_word senses();

	virtual EC_word plterm() override;
};


template<>
class Semantics<ExogAction>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ExogAction>
{
public:
	using AbstractSemantics<ExogAction>::AbstractSemantics;
	virtual ~Semantics() override = default;

	EC_word exog_action();
	vector<EC_word> causes_vals();
	EC_word poss();

	virtual EC_word plterm() override;
};


template<>
class Semantics<ExogEvent>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ExogEvent>
{
public:
	using AbstractSemantics<ExogEvent>::AbstractSemantics;
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
};


template<>
class Semantics<Activity>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Activity>
{
public:
	using AbstractSemantics<Activity>::AbstractSemantics;
	virtual ~Semantics() override = default;

	const Activity &activity();

	virtual EC_word plterm() override;
	EC_word sensing_result();
};


template<>
class Semantics<Transition>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Transition>
{
public:
	using AbstractSemantics<Transition>::AbstractSemantics;
	virtual ~Semantics() override = default;

	const Transition &trans();

	virtual EC_word plterm() override;
};


} // namespace gologpp


#endif
