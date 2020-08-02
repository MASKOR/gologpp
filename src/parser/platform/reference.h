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

#include <parser/utilities.h>


namespace gologpp {
namespace parser {


template<class PlatformT>
rule<platform::Reference<PlatformT> *(Scope &)> &platform_ref();


template<class GologT>
struct PlatformRefParser : grammar<platform::Reference<GologT> *(Scope &, const Type &)> {
	PlatformRefParser();

	rule<platform::Reference<GologT> *(Scope &, const Type &)> start;
	rule<platform::Reference<GologT> *(Scope &, const Type &), locals<TypeList>> pred_ref;
	rule<vector<Expression *>(Scope &, TypeList), locals<TypeList>> ref_args;
	rule<Expression *(const Type &)> value_or_wildcard;
};




}
}
