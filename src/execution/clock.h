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

#include <chrono>

namespace gologpp {


struct Clock {
	using rep = double;
	using duration = std::chrono::duration<rep, std::nano>;
	using period = duration::period;
	using time_point = std::chrono::time_point<Clock, Clock::duration>;
	static constexpr bool is_steady = true;

	static PlatformBackend *clock_source;

	static time_point now() noexcept;

	struct DurationRange {
		DurationRange();
		DurationRange(Clock::rep min, Clock::rep max);
		duration min;
		duration max;
	};
};





string to_string(Clock::time_point tp);

string to_string(Clock::duration d);

} // namespace gologpp
