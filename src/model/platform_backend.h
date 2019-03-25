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
	using ActivitySet = std::unordered_set<
		shared_ptr<Grounding<Action>>,
		Grounding<Action>::Hash,
		Grounding<Action>::Equals
	>;
	virtual ~PlatformBackend();

	shared_ptr<Activity> end_activity(shared_ptr<Transition>);
	void start_activity(shared_ptr<Transition>);
	virtual void preempt_activity(shared_ptr<Transition>) = 0;

	virtual Clock::time_point time() const noexcept = 0;
	void set_context(AExecutionContext *ctx);

protected:
	void update_activity(shared_ptr<Transition>, AbstractConstant *sensing_result = nullptr);

private:
	virtual void execute_activity(shared_ptr<Activity> a) = 0;

	ActivitySet activities_;
	AExecutionContext *exec_ctx_ = nullptr;
	std::mutex mutex_;
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
		shared_ptr<Grounding<Action>>, shared_ptr<ActivityThread>,
		Grounding<Action>::Hash, Grounding<Action>::Equals
	> activity_threads_;
};




} // namespace gologpp

#endif // GOLOGPP_PLATFORM_BACKEND_H_
