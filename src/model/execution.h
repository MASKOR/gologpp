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

namespace gologpp {

class Situation;
class History;



class HistoryImplementation : public AbstractImplementation {
public:
	HistoryImplementation(History &history);
	virtual ~HistoryImplementation();

	virtual void append_exog(ExogTransition &&) = 0;

protected:
	History &history_;
};



class History : public LanguageElement<History> {
public:
	History();

	DEFINE_IMPLEMENT

	HistoryImplementation &abstract_impl()
	{ return static_cast<HistoryImplementation &>(*impl_); }

	virtual string to_string(const string &pfx) const override;
};



class AExecutionContext {
public:
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

protected:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	std::queue<ExogTransition> exog_queue_;
};



class AExecutionBackend {
public:
	virtual ~AExecutionBackend();

	virtual void execute_transition(const Transition &) = 0;
};



class ExecutionContext : public AExecutionContext {
public:
	ExecutionContext(unique_ptr<Implementor> &&implementor, unique_ptr<AExecutionBackend> &&exec_backend);

	virtual ~ExecutionContext() override;

	virtual History run(Block &&program) override;

private:
	unique_ptr<Implementor> implementor_;
	unique_ptr<AExecutionBackend> exec_backend_;
};



} // namespace gologpp

#endif
