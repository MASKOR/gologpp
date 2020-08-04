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

#include <semantics/platform/taptenc/semantics.h>

#include <taptenc/src/encoder/enc_interconnection_info.h>


namespace gologpp {


template<>
class Semantics<platform::Constraint>
: public GeneralSemantics<ModelElement>
{
public:
	unique_ptr<taptenc::encICInfo> &compile();

private:
	unique_ptr<taptenc::encICInfo> representation_;
};


} // namespace gologpp
