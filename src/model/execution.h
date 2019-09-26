#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include "utilities.h"
#include "gologpp.h"
#include "platform_backend.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

namespace gologpp {



class AExecutionContext {
public:
	typedef std::queue<shared_ptr<Grounding<AbstractAction>>> ExogQueue;

	AExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&platform_backend);
	virtual ~AExecutionContext() = default;

	virtual bool final(Block &program, History &h) = 0;
	virtual bool trans(Block &program, History &h) = 0;

	/**
	 * @brief compile called once for the toplevel @param program.
	 */
	virtual void precompile() = 0;
	virtual void compile(const Block &program) = 0;
	virtual void compile(const Fluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const Function &function) = 0;
	virtual void postcompile() = 0;

	virtual History run(Block &&program) = 0;

	shared_ptr<Grounding<AbstractAction>> exog_queue_pop();
	shared_ptr<Grounding<AbstractAction>> exog_queue_poll();
	bool exog_empty();
	void exog_queue_push(shared_ptr<Grounding<AbstractAction>> exog);

	SemanticsFactory &semantics_factory();

	PlatformBackend &backend();

private:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	ExogQueue exog_queue_;
	unique_ptr<PlatformBackend> platform_backend_;
	unique_ptr<SemanticsFactory> semantics_;
};



class ExecutionContext : public AExecutionContext {
public:
	ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&exec_backend);

	virtual ~ExecutionContext() override;

	virtual History run(Block &&program) override;

	Clock::time_point context_time() const;

private:
	Clock::time_point context_time_;
};




} // namespace gologpp

#endif
