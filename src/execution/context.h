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

#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include <model/utilities.h>
#include <model/gologpp.h>

#include "platform_backend.h"
#include "history.h"
#include "transformation.h"

#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>

namespace gologpp {


class Terminate {
};


class NotPossible {
};



class AExecutionContext {
public:
	typedef std::queue<shared_ptr<Reference<AbstractAction>>> ExogQueue;

	AExecutionContext(unique_ptr<PlatformBackend> &&platform_backend);
	virtual ~AExecutionContext() = default;

	virtual void precompile() = 0;

	/// Called once for the toplevel @param program.
	virtual void compile(const Block &program) = 0;
	virtual void compile(const Fluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const Function &function) = 0;
	virtual void compile(const Procedure &function) = 0;

	virtual void postcompile() = 0;

	virtual void run(Block &&program) = 0;

	/// \return Head popped from the exog_queue or nullptr if it is empty.
	shared_ptr<Reference<AbstractAction>> exog_queue_pop();

	/// Block until the exog_queue is non-empty or exog_timer_wakeup() is called.
	/// \return Head popped from the exog_queue or nullptr if exog_timer_wakeup() was called.
	shared_ptr<Reference<AbstractAction>> exog_queue_poll();

	bool exog_empty();
	void exog_queue_push(shared_ptr<Reference<AbstractAction>> exog);

	/// Update context_time and unblock exog_queue.
	void exog_timer_wakeup();

	virtual void terminate();

	SemanticsFactory &semantics_factory() const;
	PlatformBackend &backend();
	History &history();

	void drain_exog_queue();
	void drain_exog_queue_blocking();

	void set_silent(bool silent);
	bool silent() const;

	/// \return The time the ExecutionContext is currently working with.
	/// Updated intermittently by exogenous timer events.
	Clock::time_point context_time() const;

	shared_ptr<platform::SwitchStateAction> switch_state_action();

private:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	std::mutex wait_mutex_;

	ExogQueue exog_queue_;
	unique_ptr<PlatformBackend> platform_backend_;
	SemanticsFactory *semantics_;
	History history_;
	bool silent_;
	shared_ptr<ExogAction> step_time_action_;
	shared_ptr<platform::SwitchStateAction> switch_state_action_;
	unique_ptr<Clock::time_point> context_time_;

protected:
	std::atomic_bool terminated;
};



class ExecutionContext : public AExecutionContext {
public:
	ExecutionContext(
		unique_ptr<PlatformBackend> &&exec_backend,
		unique_ptr<PlanTransformation> &&plan_transformation
	);

	virtual ~ExecutionContext() override;

	virtual void run(Block &&program) override;

private:
	unique_ptr<PlanTransformation> plan_transformation_;
};




} // namespace gologpp

#endif
