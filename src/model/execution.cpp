#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"
#include "history.h"

#include <iostream>
#include <iomanip>

namespace gologpp {


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



PlatformBackend::~PlatformBackend()
{}


ExecutionContext::ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&exec_backend)
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
