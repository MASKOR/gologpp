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

#ifndef GOLOGPP_DUMMY_BACKEND_H_
#define GOLOGPP_DUMMY_BACKEND_H_

#include "platform_backend.h"

#include <random>
#include <functional>

namespace gologpp {



class DummyBackend : public PlatformBackend {
public:
	DummyBackend();

	virtual void preempt_activity(shared_ptr<Activity>) override;
	virtual Clock::time_point time() const noexcept override;

	virtual Value eval_exog_function(
		const Type &return_type,
		const string &backend_name,
		const std::unordered_map<string, Value> &args
	) override;

private:
	virtual void terminate_() override;
	virtual void execute_activity(shared_ptr<Activity> a) override;

	std::function<void()> rnd_exog_generator();
	unique_ptr<Value> rnd_value(const Type &);

	std::mt19937 prng_;

	static const string poss_chars_;

	std::uniform_real_distribution<> rnd_activity_duration_;
	std::uniform_real_distribution<> rnd_exog_delay_;
	std::uniform_int_distribution<> rnd_exog_index_;
	std::uniform_int_distribution<> rnd_char_;
	std::uniform_int_distribution<> rnd_strlen_;
	std::uniform_int_distribution<> rnd_list_len_;
	std::uniform_real_distribution<> rnd_number_value_;
	std::uniform_int_distribution<> rnd_bool_value_;

	std::mutex thread_mtx_;

	vector<shared_ptr<ExogAction>> exogs_;
	std::thread exog_generator_;

	class ActivityThread {
	public:
		ActivityThread(DummyBackend &backend, shared_ptr<Activity> a, double duration);
		~ActivityThread() = default;
		void start();
		void cancel();

	private:
		std::thread thread_;
		std::condition_variable cancel_cond_;
		std::atomic_bool cancel_;
		std::mutex cancel_mutex_;

		DummyBackend &backend_;
		std::chrono::duration<double> duration_;
		shared_ptr<Activity> activity_;
	};

	std::unordered_map<
		shared_ptr<Activity>, shared_ptr<ActivityThread>
	> activity_threads_;
};



} // namespace gologpp

#endif // GOLOGPP_DUMMY_BACKEND_H_
