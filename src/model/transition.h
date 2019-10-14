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

#ifndef GOLOGPP_TRANSITION_H_
#define GOLOGPP_TRANSITION_H_

#include "gologpp.h"
#include "reference.h"
#include "action.h"


namespace gologpp {


class Transition : public Grounding<Action>, public LanguageElement<Transition> {
public:
	enum Hook { START, STOP, FINISH, FAIL };

	Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, Hook hook);

	Hook hook() const;
	virtual string to_string(const string &pfx) const override;
	virtual void attach_semantics(SemanticsFactory &) override;

private:
	Hook hook_;
};


string to_string(Transition::Hook);


}

#endif // GOLOGPP_TRANSITION_H_
