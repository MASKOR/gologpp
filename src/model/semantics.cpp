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

#include "semantics.h"
#include "platform/semantics.h"


namespace gologpp {


SemanticsFactory::SemanticsFactory(unique_ptr<platform::SemanticsFactory> &&psf)
: platform_semantics_factory_(std::move(psf))
{}

AExecutionContext &SemanticsFactory::context()
{ return *context_; }


void SemanticsFactory::set_context(AExecutionContext &ctx)
{
	context_ = &ctx;
	platform_semantics_factory_->set_context(ctx);
}

platform::SemanticsFactory &SemanticsFactory::platform_semantics_factory()
{ return *platform_semantics_factory_; }


SemanticsFactory &SemanticsFactory::get() {
	static unique_ptr<SemanticsFactory> instance = construct();
	return *instance;
}


} // namespace gologpp
