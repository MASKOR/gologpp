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
	virtual void preempt_activity(shared_ptr<Transition>) = 0;
	Lock lock();

	virtual Clock::time_point time() const noexcept = 0;
	void set_context(AExecutionContext *ctx);


private:
	virtual void execute_activity(shared_ptr<Activity> a) = 0;

	ActivitySet activities_;
	AExecutionContext *exec_ctx_ = nullptr;
	Lock::mutex_type mutex_;
};



class DummyBackend : public PlatformBackend {
public:
	DummyBackend();

	virtual void preempt_activity(shared_ptr<Transition>) override;
	virtual Clock::time_point time() const noexcept override;

private:
	virtual void execute_activity(shared_ptr<Activity> a) override;

	std::uniform_real_distribution<> uniform_dist_;
	std::mt19937 prng_;
	std::mutex thread_mtx_;

	struct ActivityThread {
	public:
		std::unique_ptr<std::thread> thread;
		std::condition_variable cancel_cond;
		std::mutex cancel_mutex;
		std::atomic_bool cancel;
		void end_activity(std::chrono::duration<double> when, DummyBackend &b, shared_ptr<Activity> a);
	};

	std::unordered_map<
		shared_ptr<Grounding<Action>>, shared_ptr<ActivityThread>
	> activity_threads_;
};




} // namespace gologpp

#endif // GOLOGPP_PLATFORM_BACKEND_H_
