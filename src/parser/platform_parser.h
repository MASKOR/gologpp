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
 * along with golog++.	If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef GOLOGPP_PARSER_PLATFORM_PARSER_H_
#define GOLOGPP_PARSER_PLATFORM_PARSER_H_

#include <model/platform_model.h>
#include <model/platform_constraints.h>
#include "utilities.h"

namespace gologpp {
namespace parser {

	unique_ptr<PlatformModel> parse_platform_model(const std::string &filename);
	vector<shared_ptr<PlatformConstraint>> parse_constraints(const std::string &filename);

} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_PLATFORM_PARSER_H_