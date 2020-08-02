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

#ifndef GOLOGPP_PLATFORM_BACKEND_H_
#define GOLOGPP_PLATFORM_BACKEND_H_

#include <model/gologpp.h>
#include <model/action.h>
#include <model/reference.h>

#include "transition.h"
#include "activity.h"
#include "clock.h"

#include <random>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <atomic>

namespace gologpp {


class PlatformBackend {
public:
	using ActivityMap = std::unordered_map <
		size_t,
		shared_ptr<Activity>
	>;
	using Lock = std::unique_lock<std::recursive_mutex>;

	PlatformBackend();

	virtual ~PlatformBackend();

	shared_ptr<Activity> end_activity(const Transition &);
	shared_ptr<Activity> start_activity(const Transition &);
	void cancel_activity(const Transition &);
	Lock lock();

	virtual Clock::time_point time() const noexcept = 0;
	void set_context(AExecutionContext *ctx);
	AExecutionContext *exec_context();

	Activity::State current_state(const ReferenceBase<Action> &);

	void terminate_components();
	virtual void terminate() = 0;
	virtual void schedule_timer_event(Clock::time_point when) = 0;

	void register_component_backend(const string &component_name, platform::ComponentBackend *b);
	virtual platform::ComponentBackend *get_component_backend(const string &component_name);

protected:
	void wait_until_ready();

private:
	virtual void execute_activity(shared_ptr<Activity> a) = 0;
	virtual void preempt_activity(shared_ptr<Activity> a) = 0;

	ActivityMap activities_;
	std::atomic<AExecutionContext *> exec_ctx_;
	Lock::mutex_type mutex_;

	std::mutex ready_mutex_;
	std::condition_variable ready_condition_;

	std::unordered_map<string, unique_ptr<platform::ComponentBackend>> component_backends_;
};



} // namespace gologpp

#endif // GOLOGPP_PLATFORM_BACKEND_H_
