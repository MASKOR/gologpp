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

};



template<class ImplementorT>
class ExecutionContext {
public:
	ExecutionContext() = default;
	virtual ~ExecutionContext() = default;

	virtual bool final(Block &program, History &h) = 0;
	virtual bool trans(Block &program, History &h) = 0;

	virtual void compile(const Block &program) = 0;
	virtual void compile(const AbstractFluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const AbstractFunction &function) = 0;


	ExogTransition exog_queue_pop()
	{
		std::lock_guard<std::mutex> { exog_mutex_ };
		ExogTransition rv = std::move(exog_queue_.front());
		exog_queue_.pop();
		return rv;
	}


	ExogTransition exog_queue_poll()
	{
		std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
		queue_empty_condition_.wait(queue_empty_lock, [&] { return !exog_queue_.empty(); });
		return exog_queue_pop();
	}


	void exog_queue_push(ExogTransition &&exog)
	{
		std::lock_guard<std::mutex> { exog_mutex_ };
		exog_queue_.push(std::move(exog));
		{
			std::lock_guard<std::mutex> { queue_empty_mutex_ };
			queue_empty_condition_.notify_one();
		}
	}


	History run(Block &&program)
	{
		ImplementorT implementor;

		History history;
		history.implement(implementor);

		global_scope().implement_globals(implementor);

		program.implement(implementor);
		compile(program);

		while (!final(program, history)) {
			while (!exog_queue_.empty()) {
				ExogTransition exog = exog_queue_pop();
				exog.implement(implementor);
				history.abstract_impl().append_exog(std::move(exog));
			}
			if (!trans(program, history)) {
				ExogTransition exog = exog_queue_poll();
				exog.implement(implementor);
				history.abstract_impl().append_exog(std::move(exog));
			}
		}
		return history;
	}


protected:
	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	std::queue<ExogTransition> exog_queue_;

};



} // namespace gologpp

#endif
