#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"
#include "history.h"

#include <iostream>
#include <iomanip>

namespace gologpp {


AExecutionContext::AExecutionContext(unique_ptr<PlatformBackend> &&platform_backend)
: exec_backend_(move(platform_backend))
{
	if (!exec_backend_)
		exec_backend_ = std::make_unique<COutBackend>();
}


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

unique_ptr<PlatformBackend> &AExecutionContext::backend()
{ return exec_backend_;}



ExecutionContext::ExecutionContext(unique_ptr<SemanticsFactory> &&implementor, unique_ptr<PlatformBackend> &&exec_backend)
: AExecutionContext(std::move(exec_backend))
, implementor_(std::move(implementor))
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
		else {
			shared_ptr <Transition> trans = history.abstract_impl().get_last_transition();
			trans->state = Action::IDLE;
			backend()->set_running_transition(trans);
			backend()->execute_transition(*trans);
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



PlatformBackend::~PlatformBackend()
{}

std::unordered_set< shared_ptr<Transition> >& PlatformBackend::running_transition()
{ return running_transitions_; }

void PlatformBackend::set_running_transition(shared_ptr <Transition> trans)
{ running_transitions_.insert(trans); }



void COutBackend::execute_transition(const Transition &t)
{ std::cout << "<<< Transition " << t.str() << " >>>" << std::endl; }


}
