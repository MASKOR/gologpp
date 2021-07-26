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

#include <execution/transition.h>

namespace gologpp {


template<>
class Semantics<Transition>
: public GeneralSemantics<Transition>
{
public:
	using GeneralSemantics<Transition>::GeneralSemantics;

	virtual GeneralSemantics<Transition> *copy(const Transition &target_element) const override;
	virtual unique_ptr<Plan> trans(const BindingChain &b, History &h) override;

	virtual bool final(const BindingChain &, const History &) override
	{ throw Bug("This method should not be called"); }
};


} // namespace gologpp
