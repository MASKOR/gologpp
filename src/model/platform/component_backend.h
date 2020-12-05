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

#pragma once

#include <model/gologpp.h>
#include <model/platform/component.h>

#include <atomic>
#include <condition_variable>
#include <thread>
#include <random>


namespace gologpp {
namespace platform {

class ComponentBackend {
public:
	ComponentBackend(std::initializer_list<string> supported_states);

	void set_model(Component &model);
	Component &model();
	void set_context(AExecutionController &context);
	AExecutionController &context();

	void exog_state_change(const string &state_name);
	void exog_state_change(const shared_ptr<State> &state);

	void terminate();

	virtual void switch_state(const string &state_name) = 0;
	virtual void init() = 0;
	virtual void handle_missed_transition();

protected:
	virtual void terminate_() = 0;
	std::atomic_bool terminated;

private:
	Component *model_;
	AExecutionController *exec_context_;
	std::set<string> supported_states_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class DummyComponentBackend : public ComponentBackend {
public:
	DummyComponentBackend();

	virtual void switch_state(const string &state_name) override;
	virtual void init() override;

	std::function<void()> exog_state_changer();

	void request_state_change(const string &state);
	void request_cancel();
	shared_ptr<State> requested_state();

	virtual void handle_missed_transition() override;

protected:
	virtual void terminate_() override;

private:
	std::mt19937 prng_;
	std::uniform_real_distribution<> rnd_exog_trans_delay_;

	std::thread exog_state_change_thread_;

	std::condition_variable pending_request_;
	std::condition_variable request_delay_;
	std::recursive_mutex request_mutex_;
	shared_ptr<State> requested_state_;
};


} // namespace platform
} // namespace gologpp
