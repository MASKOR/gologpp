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

#include "gologpp.h"
#include "action.h"
#include "reference.h"

#include <random>
#include <chrono>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <atomic>

namespace gologpp {


struct Clock {
	using rep = double;
	using duration = std::chrono::duration<rep, std::nano>;
	using period = duration::period;
	using time_point = std::chrono::time_point<Clock, Clock::duration>;
	static constexpr bool is_steady = true;

	static PlatformBackend *clock_source;

	static time_point now() noexcept;
};



class PlatformBackend {
public:
	using ActivitySet = std::unordered_set<shared_ptr<Grounding<Action>>>;
	using Lock = std::unique_lock<std::recursive_mutex>;

	virtual ~PlatformBackend();

	shared_ptr<Activity> end_activity(shared_ptr<Transition>);
	void start_activity(shared_ptr<Transition>);
	void cancel_activity(shared_ptr<Transition>);
	Lock lock();

	virtual Clock::time_point time() const noexcept = 0;
	void set_context(AExecutionContext *ctx);
	AExecutionContext *exec_context();

	virtual void terminate() = 0;

protected:
	void wait_until_ready();

private:
	virtual void execute_activity(shared_ptr<Activity> a) = 0;
	virtual void preempt_activity(shared_ptr<Activity> a) = 0;

	ActivitySet activities_;
	std::atomic<AExecutionContext *> exec_ctx_;
	Lock::mutex_type mutex_;

	std::mutex ready_mutex_;
	std::condition_variable ready_condition_;
};



} // namespace gologpp

#endif // GOLOGPP_PLATFORM_BACKEND_H_
