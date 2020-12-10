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
public:
	friend PlatformBackend;

	using seconds = std::chrono::duration<double, std::ratio<1,1>>;
	using duration = seconds;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = std::chrono::time_point<Clock>;
	static constexpr bool is_steady = true;

	static time_point now() noexcept;
	static void init();
	static void uninit();

	static bool ready();

	struct DurationRange {
		DurationRange();
		DurationRange(Clock::rep min, Clock::rep max);
		duration min;
		duration max;
	};


	static time_point epoch();

private:
	static void set_clock_source(PlatformBackend *);
	static PlatformBackend *clock_source_;
	static time_point epoch_;
};


string to_string(Clock::time_point tp);

string to_string(Clock::duration d);


} // namespace gologpp
