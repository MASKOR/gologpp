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


namespace gologpp {
namespace platform {

class ComponentBackend {
public:
	void set_model(Component &model);
	Component &model();
	void set_context(AExecutionContext &context);
	AExecutionContext &context();

	void exog_state_change(const string &state_name);

	virtual void switch_state(const string &state_name) = 0;
	virtual void init() = 0;
	virtual void terminate() = 0;
	virtual bool is_dummy() const;

private:
	Component *model_;
	AExecutionContext *exec_context_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////



class DummyComponentBackend : public ComponentBackend {
public:
	DummyComponentBackend();

	virtual void switch_state(const string &state_name) override;
	virtual void init() override;
	virtual void terminate() override;
	virtual bool is_dummy() const override;


	void request_state_change(const string &state);

private:
	std::thread exog_state_change_thread_;
	std::atomic_bool terminated_;

	std::condition_variable pending_request_;
	std::mutex mutex_;
	shared_ptr<State> requested_state_;
};


} // namespace platform
} // namespace gologpp
