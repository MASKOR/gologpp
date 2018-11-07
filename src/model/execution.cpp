#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"

#include <iostream>
#include <iomanip>

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


AExecutionContext::Clock &AExecutionContext::clock()
{ return clock_; }

AExecutionContext::ExogQueue &AExecutionContext::exog_queue()
{ return exog_queue_; }

AExecutionContext::AExecutionContext(unique_ptr<AExecutionBackend> &&exec_backend)
: exec_backend_(move(exec_backend))
{}



ExecutionContext::ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<AExecutionBackend> &&exec_backend)
: AExecutionContext(std::move(exec_backend))
, implementor_(std::move(implementor))
{}

ExecutionContext::~ExecutionContext()
{}



unique_ptr<AExecutionBackend> &AExecutionContext::backend()
{ return exec_backend_;}

std::unordered_set< shared_ptr<Transition> >& AExecutionBackend::running_transition()
{ return running_transitions_; }

void AExecutionBackend::set_running_transition(shared_ptr <Transition> trans)
{ running_transitions_.insert(trans); }

AExecutionBackend::~AExecutionBackend()
{}


History ExecutionContext::run(Block &&program)
{
	History history;
	history.attach_semantics(*implementor_);

	global_scope().implement_globals(*implementor_, *this);

	program.attach_semantics(*implementor_);
	compile(program);

	Clock::time_point t_loop = clock().now();

	while (!final(program, history)) {
		while (!exog_queue().empty()) {
			ExogTransition exog = exog_queue_pop();
			exog.attach_semantics(*implementor_);
			history.abstract_impl().append_exog(std::move(exog));
		}

		Clock::time_point t_trans = clock().now();

		if (!trans(program, history)) {
			ExogTransition exog = exog_queue_poll();
			exog.attach_semantics(*implementor_);
			history.abstract_impl().append_exog(std::move(exog));
		}
		else {
			Transition trans = history.abstract_impl().get_last_transition();
			exec_backend_->execute_transition(trans);
		}

		std::chrono::duration<double> d_trans = clock().now() - t_trans;
		std::cout << std::fixed << std::setprecision(9) << "Transition time: "
			<< d_trans.count() << " s." << std::endl;

	}

	std::chrono::duration<double> d_loop = clock().now() - t_loop;

	std::cout << std::fixed << "Mainloop time: "
		<< d_loop.count()  << " s." << std::endl;

	return history;
}




}
