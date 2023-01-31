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

#include <condition_variable>

namespace gologpp {


class Activity {
public:
	enum State { IDLE, RUNNING, FINAL, CANCELLED, FAILED };

	Activity(
		const shared_ptr<Action> &action,
		vector<unique_ptr<Value>> &&args,
		AExecutionController &, State state = IDLE
	);

	Activity(const Transition &, AExecutionController &);

	void update(Transition::Hook hook);
	void wait_for_update();

	State state() const;
	void set_state(State s);
	const std::string &mapped_name() const;
	Value mapped_arg_value(const string &name) const;
	shared_ptr<Action> action() const;
	vector<unique_ptr<Value>> args() const;
	const Grounding<Action> &ref() const;
	Grounding<Action> &ref();

	static State target_state(Transition::Hook);

private:
	Grounding<Action> action_ref_;

	State state_;
	AExecutionController &exec_context_;
	std::condition_variable update_condition_;
	std::mutex update_mutex_;
};



string to_string(Activity::State s);



} // namespace gologpp

#endif // GOLOGPP_ACTIVITY_H_
