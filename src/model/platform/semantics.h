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

#pragma once

#include <model/gologpp.h>
#include <model/semantics.h>


namespace gologpp {
namespace platform {


class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;
	static unique_ptr<SemanticsFactory> construct();

	virtual AExecutionController &context();
	void set_context(AExecutionController &);

	virtual PlanTransformation *make_transformation() = 0;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_PLATFORM_ELEMENTS)

private:
	AExecutionController *context_;
};


class DummySemanticsFactory : public SemanticsFactory {
public:
	virtual PlanTransformation *make_transformation() override;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_PLATFORM_ELEMENTS)
};


} // namespace platform
} // namespace gologpp
