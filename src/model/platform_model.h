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

#ifndef GOLOGPP_PLATFORM_MODEL_H_
#define GOLOGPP_PLATFORM_MODEL_H_

#include "gologpp.h"

#include <vector>
#include <string>

#include <boost/fusion/include/adapt_struct.hpp>

namespace gologpp {

struct Duration {
		int lb;
		int ub;
};
struct PlatformState {
	 std::string id;
	 Duration dur;
};
struct PlatformTransition {
		std::string src_id;
		std::string dst_id;
		Duration dur;
};
class PlatformModel {
	public:
		PlatformModel(std::vector<PlatformState> &arg_states, std::vector<PlatformTransition> &arg_transitions)
		:  states(arg_states), transitions(arg_transitions)
		{
			 curr_state = states.begin();
		};
	private:
		vector<PlatformState> states;
		vector<PlatformTransition> transitions;
		vector<PlatformState>::iterator curr_state;
};

} // namespace gologpp

BOOST_FUSION_ADAPT_STRUCT(
	gologpp::Duration,
	(int, lb)
	(int, ub)
)
BOOST_FUSION_ADAPT_STRUCT(
	gologpp::PlatformState,
	(std::string, id)
	(gologpp::Duration, dur)
)
BOOST_FUSION_ADAPT_STRUCT(
	gologpp::PlatformTransition,
	(std::string, src_id)
	(std::string, dst_id)
	(gologpp::Duration, dur)
)
#endif // GOLOGPP_PLATFORM_MODEL_H_
