#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"

namespace gologpp {

HistorySemantics::HistorySemantics(History &h)
: history_(h)
{}

HistorySemantics::~HistorySemantics()
{}

History::History()
{}

Scope &History::parent_scope()
{ return global_scope(); }

const Scope &History::parent_scope() const
{ return global_scope(); }


string History::to_string(const string &pfx) const
{ return pfx + linesep + "history: no representation" + linesep; }


ExogTransition AExecutionContext::exog_queue_pop()
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	ExogTransition rv = std::move(exog_queue_.front());
	exog_queue_.pop();
	return rv;
}


ExogTransition AExecutionContext::exog_queue_poll()
{
	std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
	queue_empty_condition_.wait(queue_empty_lock, [&] { return !exog_queue_.empty(); });
	return exog_queue_pop();
}


void AExecutionContext::exog_queue_push(ExogTransition &&exog)
{
	std::lock_guard<std::mutex> { exog_mutex_ };
	exog_queue_.push(std::move(exog));
	{
		std::lock_guard<std::mutex> { queue_empty_mutex_ };
		queue_empty_condition_.notify_one();
	}
}


AExecutionBackend::~AExecutionBackend()
{}


ExecutionContext::ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<AExecutionBackend> &&exec_backend)
: implementor_(std::move(implementor))
, exec_backend_(std::move(exec_backend))
{}

ExecutionContext::~ExecutionContext()
{}


History ExecutionContext::run(Block &&program)
{
	History history;
	history.attach_semantics(*implementor_);

	global_scope().implement_globals(*implementor_, *this);

	program.attach_semantics(*implementor_);
	compile(program);

	while (!final(program, history)) {
		while (!exog_queue_.empty()) {
			ExogTransition exog = exog_queue_pop();
			exog.attach_semantics(*implementor_);
			history.abstract_impl().append_exog(std::move(exog));
		}
		if (!trans(program, history)) {
			ExogTransition exog = exog_queue_poll();
			exog.attach_semantics(*implementor_);
			history.abstract_impl().append_exog(std::move(exog));
		}
	}
	return history;
}




}
