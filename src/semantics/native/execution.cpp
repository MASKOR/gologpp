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

#include "execution.h"

#include <experimental/filesystem>

#include "action.h"
#include "procedural.h"
#include "fluent.h"
#include "utilities.h"
#include "history.h"

#include <model/action.h>

namespace filesystem = std::experimental::filesystem;

namespace gologpp {


unique_ptr<NativeContext> NativeContext::instance_;


void NativeContext::init(unique_ptr<PlatformBackend> &&exec_backend)
{ instance_ = unique_ptr<NativeContext>(new NativeContext(std::move(exec_backend))); }

void NativeContext::shutdown()
{
	instance_.reset();
}


NativeContext::NativeContext(unique_ptr<PlatformBackend> &&exec_backend)
: AExecutionController(std::move(exec_backend))
{
	// TODO
}


NativeContext::~NativeContext()
{}


NativeContext &NativeContext::instance()
{ return *instance_; }


void NativeContext::compile(const Fluent &fluent)
{
}


void NativeContext::compile(const Action &aa)
{
}


void NativeContext::compile(const ExogAction &action)
{
}


void NativeContext::compile(const Function &function)
{
}


void NativeContext::compile(const ExogFunction &function)
{
}


void NativeContext::postcompile()
{
}


void NativeContext::run(const Instruction &program)
{

}



} // namespace gologpp
