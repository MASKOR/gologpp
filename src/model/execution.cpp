#include "execution.h"
#include "fluent.h"
#include "action.h"
#include "procedural.h"

namespace gologpp {

HistoryImplementation::HistoryImplementation(History &h)
: history_(h)
{}


HistoryImplementation::~HistoryImplementation()
{}


History::History()
{}


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

}
