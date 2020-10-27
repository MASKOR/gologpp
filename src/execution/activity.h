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

#ifndef GOLOGPP_ACTIVITY_H_
#define GOLOGPP_ACTIVITY_H_

#include <model/gologpp.h>
#include <model/reference.h>
#include <model/action.h>
#include "transition.h"

#include <boost/optional.hpp>


namespace gologpp {


class Activity
: public ReferenceBase<Action>
, public Reference<AbstractAction>
, public Instruction
, public LanguageElement<Activity, VoidType>
, public std::enable_shared_from_this<Activity> {
public:
	enum State { IDLE, RUNNING, FINAL, CANCELLED, FAILED };

	Activity(const shared_ptr<Action> &action, vector<unique_ptr<Expression>> &&args, AExecutionController &, State state = IDLE);
	Activity(const Transition &, AExecutionController &);

	virtual const Action &operator * () const override;
	virtual Action &operator * () override;
	virtual const Action *operator -> () const override;
	virtual Action *operator -> () override;

	State state() const;
	void set_state(State s);

	void update(Transition::Hook hook);

	const std::string &mapped_name() const;
	Value mapped_arg_value(const string &name) const;

	virtual string to_string(const string &pfx) const override;

	virtual void attach_semantics(SemanticsFactory &) override;

	static State target_state(Transition::Hook);

private:
	State state_;
	AExecutionController &exec_context_;
};



string to_string(Activity::State s);



} // namespace gologpp

#endif // GOLOGPP_ACTIVITY_H_
