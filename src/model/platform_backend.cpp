#include "platform_backend.h"
#include "execution.h"

#include <thread>
#include <iostream>
#include <tuple>

namespace gologpp {

PlatformBackend *Clock::clock_source = nullptr;

Clock::time_point Clock::now() noexcept
{ return clock_source->time(); }


static Activity::State trans2state(Transition::Hook hook) {
	switch (hook) {
	case Transition::Hook::START:
		return Activity::State::RUNNING;
	case Transition::Hook::STOP:
		return Activity::State::PREEMPTED;
	case Transition::Hook::FINISH:
		return Activity::State::FINAL;
	case Transition::Hook::FAIL:
		return Activity::State::FAILED;
	}
	throw Bug("Unhandled Transition hook. Ignored warnings when compiling?");
}


PlatformBackend::~PlatformBackend()
{}

void PlatformBackend::start_activity(shared_ptr<Transition> trans)
{
	std::lock_guard<std::mutex> locked(mutex_);
	shared_ptr<Activity> a = std::make_shared<Activity>(trans);
	execute_activity(a);
	activities_.insert(a);
}


shared_ptr<Activity> PlatformBackend::end_activity(shared_ptr<Transition> trans)
{
	std::lock_guard<std::mutex> locked(mutex_);

	ActivitySet::iterator it = activities_.find(trans);
	shared_ptr<Activity> dur_running;
	if (it != activities_.end())
		dur_running = std::dynamic_pointer_cast<Activity>(*it);

	if (!dur_running)
		throw LostTransition(trans->str());

	if (dur_running->state() != trans2state(trans->hook()))
			throw InconsistentTransition(trans->str());

	activities_.erase(it);
	std::cout << "<<< Cleared " << trans->str() << std::endl;

	return dur_running;
}


void PlatformBackend::update_activity(shared_ptr<Transition> trans, AbstractConstant *sensing_result)
{
	std::lock_guard<std::mutex> locked(mutex_);

	ActivitySet::iterator it = activities_.find(trans);
	shared_ptr<Activity> a;
	if (it != activities_.end())
		a = std::dynamic_pointer_cast<Activity>(*it);

	if (!a)
		throw LostTransition(trans->str());

	if (trans->hook() == Transition::Hook::FINISH) {
		if (a->target()->senses() && !sensing_result)
			throw Bug("PlatformBackend implementation tried to finish the sensing action "
				+ static_cast<const Grounding<Action> &>(*a).str()
				+ " without providing a sensing result"
			);
		else if (!a->target()->senses() && sensing_result)
			throw Bug("PlatformBackend implementation gave a sensing result to the action "
				+ static_cast<const Grounding<Action> &>(*a).str()
				+ ", but it is not a sensing action"
			);
		else if (sensing_result)
			a->set_sensing_result(sensing_result);
	}

	a->set_state(trans2state(trans->hook()));

	exec_ctx_->exog_queue_push(a);
}


void PlatformBackend::set_context(AExecutionContext *ctx)
{ exec_ctx_ = ctx; }



DummyBackend::DummyBackend()
: uniform_dist_(0.1, 2.0)
, prng_(1)
{}


void DummyBackend::ActivityThread::end_activity(std::chrono::duration<double> when, DummyBackend &b, shared_ptr<Activity> a)
{
	std::unique_lock<std::mutex> cancel_lock(cancel_mutex);
	bool canceled = cancel_cond.wait_for(cancel_lock, when, [&] { return bool(cancel); });

	if (canceled) {
		std::cout << "<<< Activity " << a->str() << " STOPPED >>" << std::endl;
		b.update_activity(a->transition(Transition::Hook::STOP));
	}
	else {
		std::cout << "<<< Activity " << a->str() << " FINAL >>>" << std::endl;
		b.update_activity(a->transition(Transition::Hook::FINISH));
	}

	std::lock_guard<std::mutex> locked(b.thread_mtx_);
	thread->detach();
	b.activity_threads_.erase(a);
}


void DummyBackend::execute_activity(shared_ptr<Activity> a)
{
	std::chrono::duration<double> rnd_dur { uniform_dist_(prng_) };
	std::cout << "<<< Activity " << a->str() << " START, duration: " << rnd_dur.count() << " >>>" << std::endl;

	std::lock_guard<std::mutex> locked(thread_mtx_);

	shared_ptr<ActivityThread> at = std::make_shared<ActivityThread>();
	at->thread = std::make_unique<std::thread>( [at, rnd_dur, this, a] () {
		at->end_activity(rnd_dur, *this, a);
	});

	activity_threads_[a] = at;
}


void DummyBackend::preempt_activity(shared_ptr<Transition> t)
{
	std::lock_guard<std::mutex> locked(thread_mtx_);
	if (activity_threads_.find(t) == activity_threads_.end())
		throw EngineError("No such activity: " + t->str());
	activity_threads_[t]->cancel = true;
	activity_threads_[t]->cancel_cond.notify_all();
}


Clock::time_point DummyBackend::time() const noexcept
{
	Clock::duration rv = std::chrono::steady_clock::now().time_since_epoch();
	return Clock::time_point(rv);
}



}
