/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "dummy_backend.h"
#include "activity.h"
#include "controller.h"

#include <model/logger.h>

#include <thread>
#include <iostream>
#include <tuple>
#include <array>
#include <limits>

namespace gologpp {


const string DummyBackend::poss_chars_ =
	"abcdefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"1234567890"
	"^°!§$%&/()=`+*~#,.-;:_<>| ";


DummyBackend::DummyBackend()
: prng_(1)
, rnd_activity_duration_(0.01, 0.5)
, rnd_exog_delay_(0., 2.)
, rnd_char_(0, static_cast<int>(poss_chars_.size()-1))
, rnd_strlen_(0, 255)
, rnd_list_len_(0, 16)
, rnd_number_value_(-1000, 1000)
, rnd_bool_value_(0, 1)
, terminated_(false)
{
	for (shared_ptr<Global> &g : global_scope().globals()) {
		shared_ptr<ExogAction> exog = std::dynamic_pointer_cast<ExogAction>(g);
		if (exog)
			exogs_.push_back(exog);
	}

	if (exogs_.size()) {
		rnd_exog_index_ = std::uniform_int_distribution<>(0, static_cast<int>(exogs_.size()-1));

		exog_generator_ = std::thread(rnd_exog_generator());
		exog_generator_.detach();
	}
}


std::function<void()> DummyBackend::rnd_exog_generator()
{
	return [&] () {
		wait_until_ready();
		while (!terminated_) {
			std::unique_lock<std::mutex> sleep_lock(terminate_mutex_);
			terminate_condition_.wait_for(sleep_lock, std::chrono::duration<double>(rnd_exog_delay_(prng_)));
			if (terminated_)
				break;
			shared_ptr<ExogAction> exog = exogs_[static_cast<size_t>(rnd_exog_index_(prng_))];

			vector<unique_ptr<Expression>> args;
			for (shared_ptr<Variable> p : exog->params())
				args.push_back(rnd_value(p->type()));

			shared_ptr<Reference<AbstractAction>> event(new Reference<ExogAction>(exog, std::move(args)));
			event->attach_semantics(exec_context()->semantics_factory());

			exec_context()->exog_queue_push(event);
		}
		log(LogLevel::INF) << "exog generator TERMINATED" << flush;
	};
}


unique_ptr<Value> DummyBackend::rnd_value(const Type &type)
{
	unique_ptr<Value> rv;
	if (type.is<NumberType>())
		rv.reset(new Value(type, static_cast<double>(rnd_number_value_(prng_))));
	else if (type.is<BoolType>())
		rv.reset(new Value(type, static_cast<bool>(rnd_bool_value_(prng_))));
	else if (type.is<StringType>() || type.is<SymbolType>()) {
		std::string v;
		for (size_t i = 0; i < static_cast<size_t>(rnd_strlen_(prng_)); i++)
			v.push_back(poss_chars_[static_cast<size_t>(rnd_char_(prng_))]);
		rv.reset(new Value(type, v));
	}
	else if (type.is<CompoundType>()) {
		CompoundType::Representation v_rep;
		const CompoundType &ctype = dynamic_cast<const CompoundType &>(type);
		for (const string &field_name : ctype.field_names())
			v_rep[field_name] = rnd_value(ctype.field_type(field_name));
		rv.reset(new Value(ctype, v_rep));
	}
	else if (type.is<ListType>()) {
		ListType::Representation v_rep;
		const ListType &ltype = dynamic_cast<const ListType &>(type);
		for (size_t i = 0; i < static_cast<size_t>(rnd_list_len_(prng_)); i++)
			v_rep.push_back(rnd_value(ltype.element_type()));
		rv.reset(new Value(ltype, v_rep));
	}
	else
		throw Bug("DummyBackend: Unhandled type: " + type.name());

	return rv;
}


void DummyBackend::preempt_activity(shared_ptr<Activity> a)
{
	std::lock_guard<std::mutex> locked(thread_mtx_);
	if (activity_threads_.find(a) == activity_threads_.end())
		throw EngineError("No such activity: " + a->str());
	activity_threads_[a]->cancel();
}


Clock::time_point DummyBackend::time() const noexcept
{
	return Clock::time_point(
		std::chrono::duration_cast<Clock::duration>(
			std::chrono::steady_clock::now().time_since_epoch()
		)
	);
}


void DummyBackend::terminate()
{
	terminated_ = true;
	terminate_condition_.notify_all();
	for (auto &entry : activity_threads_)
		entry.second->cancel();
}


void DummyBackend::schedule_timer_event(Clock::time_point when)
{
	timer_evt_thread_ = std::thread([&, when] () {
		wait_until_ready();

		std::unique_lock<std::mutex> sleep_lock(terminate_mutex_);
		//log(LogLevel::DBG) << "=== @" << Clock::now() << ": Schedule wakeup @" << when << flush;
		terminate_condition_.wait_until(sleep_lock, when, [&] () {
			return terminated_.load();
		} );

		if (!terminated_) {
			//log(LogLevel::DBG) << "<<< Dispatch wakeup " << when << " @" << Clock::now() << flush;
			exec_context()->exog_timer_wakeup();
		}
	});
	timer_evt_thread_.detach();
}



DummyBackend::ActivityThread::ActivityThread(DummyBackend &backend, shared_ptr<Activity> a,	double duration)
: cancel_(false)
, backend_(backend)
, duration_(duration)
, activity_(a)
{}


void DummyBackend::ActivityThread::start()
{
	activity_thread_ = std::thread(
		[&] () {
			std::unique_lock<std::mutex> cancel_lock(cancel_mutex_);
			bool canceled = cancel_cond_.wait_for(cancel_lock, duration_, [&] { return bool(cancel_); });

			if (canceled) {
				std::cout << "DummyBackend: Activity " << activity_->str() << " STOPPED" << std::endl;
				activity_->update(Transition::Hook::CANCEL);
			}
			else {
				std::cout << "DummyBackend: Activity " << activity_->str() << " FINAL" << std::endl;
				if (activity_->target()->senses()) {
					unique_ptr<Value> sensing_result = backend_.rnd_value(activity_->target()->senses()->type());
					std::cout << "Sensing result: " << sensing_result->str() << std::endl;
					activity_->update(Transition::Hook::FINISH, *sensing_result);
				}
				else
					activity_->update(Transition::Hook::FINISH);
			}

			std::lock_guard<std::mutex> locked(backend_.thread_mtx_);
			backend_.activity_threads_.erase(activity_);
		}
	);
	activity_thread_.detach();
}


void DummyBackend::ActivityThread::cancel()
{
	cancel_ = true;
	cancel_cond_.notify_all();
	activity_thread_.join();
}


void DummyBackend::execute_activity(shared_ptr<Activity> a)
{
	double rnd_dur { rnd_activity_duration_(prng_) };

	std::cout << "DummyBackend: Activity " << a->str() << " START, duration: " << rnd_dur << std::endl;

	std::cout << "   " << a->target()->mapping().backend_name() << "( ";
	for (const auto &pair : a->target()->mapping().arg_mapping()) {
		std::cout << a->mapped_arg_value(pair.first).str() << " ";
	}
	std::cout << " )" << std::endl;

	std::lock_guard<std::mutex> locked(thread_mtx_);

	shared_ptr<ActivityThread> at = std::make_shared<ActivityThread>(*this, a, rnd_dur);
	activity_threads_[a] = at;
	at->start();
}




} // namespace gologpp
