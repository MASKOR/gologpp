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

#ifndef GOLOGPP_HISTORY_H_
#define GOLOGPP_HISTORY_H_

#include "language.h"
#include "scope.h"
#include "gologpp.h"
#include "semantics.h"

namespace gologpp {


template<>
class AbstractSemantics<History> : public AbstractSemantics<AbstractLanguageElement> {
public:
	AbstractSemantics<History>(History &history);
	virtual ~AbstractSemantics<History>() override;

	virtual shared_ptr<Transition> get_last_transition() = 0;
	virtual void append_exog(shared_ptr<Grounding<AbstractAction>>) = 0;
	virtual void append_sensing_result(shared_ptr<Activity>) = 0;

	const History &element() const;

protected:
	History &history_;
};


class History : public LanguageElement<History>, public NoScopeOwner {
public:
	History() = default;
	History(History &&);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	DEFINE_ATTACH_SEMANTICS

	virtual string to_string(const string &pfx) const override;
};



} // namespace gologpp


#endif // GOLOGPP_HISTORY_H_
