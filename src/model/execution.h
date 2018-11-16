#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include "utilities.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"
#include "gologpp.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <unordered_set>

namespace gologpp {


class PlatformBackend;


class AExecutionContext {
public:
	typedef std::chrono::steady_clock Clock;
	typedef std::queue<ExogTransition> ExogQueue;

	AExecutionContext(unique_ptr<PlatformBackend> &&exec_backend);
	virtual ~AExecutionContext() = default;

	virtual bool final(Block &program, History &h) = 0;
	virtual bool trans(Block &program, History &h) = 0;

	/**
	 * @brief compile called once for the toplevel @param program.
	 */
	virtual void compile(const Block &program) = 0;
	virtual void compile(const AbstractFluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const AbstractFunction &function) = 0;

	virtual History run(Block &&program) = 0;

	ExogTransition exog_queue_pop();
	ExogTransition exog_queue_poll();
	void exog_queue_push(ExogTransition &&exog);
	Clock &clock();

	unique_ptr<PlatformBackend> &backend();

protected:
	ExogQueue &exog_queue();

private:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	ExogQueue exog_queue_;
	Clock clock_;
	unique_ptr<PlatformBackend> exec_backend_;
};



class ExecutionContext : public AExecutionContext {
public:
	ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&exec_backend);

	virtual ~ExecutionContext() override;

	virtual History run(Block &&program) override;

private:
	unique_ptr<SemanticsFactory> implementor_;
};



class PlatformBackend {
public:
	virtual ~PlatformBackend();

	virtual void execute_transition(const Transition &) = 0;
	std::unordered_set < shared_ptr <Transition> > &running_transition() ;
	void set_running_transition (shared_ptr <Transition> trans);

protected:
	std::unordered_set < shared_ptr <Transition> > running_transitions_;
	std::queue < shared_ptr <Transition> > done_transitions_;
};



class COutBackend : public PlatformBackend {
public:
	virtual void execute_transition(const Transition &) override;
};



} // namespace gologpp

#endif
