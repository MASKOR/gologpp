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

#include <execution/context.h>

namespace gologpp {
namespace platform {


void ComponentBackend::set_model(Component &model)
{ model_ = &model; }

Component &ComponentBackend::model()
{ return *model_; }

void ComponentBackend::set_context(AExecutionContext &context)
{ exec_context_ = &context; }

AExecutionContext &ComponentBackend::context()
{ return *exec_context_; }

void ComponentBackend::exog_state_change(const string &state_name)
{
	shared_ptr<State> tgt = model_->scope().lookup_identifier<State>(state_name);
	if (!tgt)
		throw ComponentError(string(__func__) + ": Invalid target state: " + state_name);

	model_->find_transition<ExogTransition>(model_->current_state(), *tgt);

	shared_ptr<SwitchStateAction> exog_state_change = exec_context_->switch_state_action();
	shared_ptr<gologpp::Reference<AbstractAction>> evt { new gologpp::Reference<platform::SwitchStateAction> {
		exog_state_change,
		{
			new Value(get_type<StringType>(), model_->name()),
			new Value(get_type<StringType>(), model_->current_state().name()),
			new Value(get_type<StringType>(), state_name)
		}
	} };

	model_->set_current_state(tgt);

	context().exog_queue_push(evt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void DummyComponentBackend::switch_state(const string &state_name)
{ log(LogLevel::INF) << "Dummy component " << model().name() << " switches to state " << state_name << flush; }

void DummyComponentBackend::init()
{ log(LogLevel::INF) << "Dummy component " << model().name() << " init" << flush; }

void DummyComponentBackend::terminate()
{ log(LogLevel::INF) << "Dummy component " << model().name() << " terminate" << flush; }



} //namespace gologpp
} //namespace platform
