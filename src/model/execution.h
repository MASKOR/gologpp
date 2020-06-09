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

#include "utilities.h"
#include "gologpp.h"
#include "platform_backend.h"
#include "history.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>

namespace gologpp {


class Terminate {
};



class AExecutionContext {
public:
	typedef std::queue<shared_ptr<Grounding<AbstractAction>>> ExogQueue;

	AExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&platform_backend);
	virtual ~AExecutionContext() = default;

	/**
	 * @brief compile called once for the toplevel @param program.
	 */
	virtual void precompile() = 0;
	virtual void compile(const Block &program) = 0;
	virtual void compile(const Fluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const Function &function) = 0;
	virtual void compile(const Procedure &function) = 0;
	virtual void postcompile() = 0;

	virtual void run(Block &&program) = 0;

	shared_ptr<Grounding<AbstractAction>> exog_queue_pop();
	shared_ptr<Grounding<AbstractAction>> exog_queue_poll();
	void wait_for_exog();
	bool exog_empty();
	void exog_queue_push(shared_ptr<Grounding<AbstractAction>> exog);

	void terminate();

	SemanticsFactory &semantics_factory();
	PlatformBackend &backend();
	History &history();

	void drain_exog_queue();
	void drain_exog_queue_blocking();

	void set_silent(bool silent);
	bool silent() const;

private:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	std::mutex wait_mutex_;
	ExogQueue exog_queue_;
	unique_ptr<PlatformBackend> platform_backend_;
	unique_ptr<SemanticsFactory> semantics_;
	History history_;
	bool silent_;

protected:
	std::atomic_bool terminated;
};



class ExecutionContext : public AExecutionContext {
public:
	ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&exec_backend);

	virtual ~ExecutionContext() override;

	virtual void run(Block &&program) override;

	Clock::time_point context_time() const;

private:
	Clock::time_point context_time_;
};




} // namespace gologpp

#endif
