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

#include <model/platform/switch_state_action.h>
#include <model/reference.h>
#include <model/logger.h>

#include "component_backend.h"

#include <execution/controller.h>

#include <iostream>

namespace gologpp {
namespace platform {


ComponentBackend::ComponentBackend(std::initializer_list<string> supported_states)
: terminated(false)
, supported_states_(supported_states)
{
	if (!supported_states_.empty())
		supported_states_.insert("error");
}

void ComponentBackend::set_model(Component &model)
{
	model_ = &model;
	if (!supported_states_.empty()) {
		for (auto &state : model_->states())
			if (supported_states_.find(state->name()) == supported_states_.end())
				throw UserError(
					"Component " + model_->name()
					+ ": State " + state->name()
					+ " is not supported by backend"
				);
	}
}

Component &ComponentBackend::model()
{ return *model_; }

void ComponentBackend::set_context(AExecutionController &context)
{ exec_context_ = &context; }

AExecutionController &ComponentBackend::context()
{ return *exec_context_; }

void ComponentBackend::exog_state_change(const string &state_name)
{
	shared_ptr<State> tgt = model_->scope().lookup_identifier<State>(state_name);
	if (!tgt)
		throw ComponentError(string(__func__) + ": Invalid target state: " + state_name);
	exog_state_change(tgt);
}

void ComponentBackend::exog_state_change(const shared_ptr<State> &tgt)
{
	if (!model_->find_transition<ExogTransition>(*model_->current_state(), *tgt))
		log(LogLevel::ERR) << "Component backend \"" << model().name() << "\" breached model by going from state \""
			<< model().current_state() << "\" to \"" << tgt->name() << "\"" << flush;

	shared_ptr<SwitchStateAction> exog_state_change = exec_context_->switch_state_action();
	shared_ptr<gologpp::Reference<AbstractAction>> evt { new gologpp::Reference<platform::SwitchStateAction> {
		exog_state_change,
		{
			new Value(get_type<StringType>(), model_->name()),
			new Value(get_type<StringType>(), model_->current_state()->name()),
			new Value(get_type<StringType>(), tgt->name())
		}
	} };

	model_->set_current_state_exog(tgt);

	context().exog_queue_push(evt);
}


void ComponentBackend::terminate()
{
	terminated = true;
	terminate_();
}

void ComponentBackend::handle_missed_transition()
{ model().set_current_state(model().error_state()); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

DummyComponentBackend::DummyComponentBackend()
: ComponentBackend({})
, prng_(1)
, rnd_exog_trans_delay_(0, 5)
{}

void DummyComponentBackend::switch_state(const string &state_name)
{
	request_cancel();
	log(LogLevel::INF) << "Dummy component " << model().name() << " switches to state " << state_name << flush;
}


void DummyComponentBackend::init()
{
	log(LogLevel::INF) << "Dummy component " << model().name() << " init" << flush;

	exog_state_change_thread_ = std::thread(exog_state_changer());
	exog_state_change_thread_.detach();
}


std::function<void ()> DummyComponentBackend::exog_state_changer()
{
	return [&] () {
		while (!terminated) {
			std::mutex m1;
			std::unique_lock<std::mutex> lock(m1);
			pending_request_.wait(lock, [&] () {
				return terminated || requested_state();
			});

			if (requested_state()) {
				gologpp::Clock::duration delay {
					std::abs(std::round(rnd_exog_trans_delay_(prng_) * 10) / 10)
				};

				std::cv_status wait_res = std::cv_status::timeout;
				if (delay > gologpp::Clock::duration(0.1)) {
					log(LogLevel::NFY) << "Dummy component " << model().name() << " delays exog state switch by "
						<< delay.count() << " seconds." << flush;
					std::mutex m2;
					std::unique_lock<std::mutex> l2(m2);
					wait_res = request_delay_.wait_for(l2, delay);
				}

				// Check again because we may have caused a timeout that took us to the error state instead
				std::lock_guard<std::recursive_mutex> l(request_mutex_);
				if (wait_res == std::cv_status::timeout && requested_state_) {
					log(LogLevel::NFY) << "Dummy component " << model().name() << " exog switches to state "
						<< requested_state_->name() << flush;
					exog_state_change(requested_state_->name());
					requested_state_.reset();
				}
			}
		}
	};
}


void DummyComponentBackend::terminate_()
{
	request_cancel();
	pending_request_.notify_all();
}


void DummyComponentBackend::request_state_change(const string &state)
{
	std::lock_guard<std::recursive_mutex> l(request_mutex_);
	requested_state_ = model().state(state);
	if (!requested_state_)
		throw Bug("Invalid state \"" + state + "\" requested on component " + model().name());
	pending_request_.notify_all();
}

void DummyComponentBackend::request_cancel()
{
	std::lock_guard<std::recursive_mutex> l(request_mutex_);
	requested_state_.reset();
	request_delay_.notify_all();
}

shared_ptr<State> DummyComponentBackend::requested_state()
{
	std::lock_guard<std::recursive_mutex> l(request_mutex_);
	return requested_state_;
}


void DummyComponentBackend::handle_missed_transition()
{
	ComponentBackend::handle_missed_transition();
	request_cancel();
}





} //namespace gologpp
} //namespace platform
