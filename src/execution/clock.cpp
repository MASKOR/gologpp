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

#include "clock.h"
#include "platform_backend.h"


namespace gologpp {


PlatformBackend *Clock::clock_source_ = nullptr;
Clock::time_point Clock::epoch_;

Clock::time_point Clock::now() noexcept
{
	return Clock::time_point(clock_source_->time() - epoch());
}

void Clock::init()
{ epoch_ = clock_source_->time(); }

void Clock::uninit()
{
	epoch_ = time_point();
	clock_source_ = nullptr;
}

bool Clock::ready()
{ return clock_source_; }

void Clock::set_clock_source(PlatformBackend *b)
{ clock_source_ = b; }

Clock::time_point Clock::epoch()
{ return epoch_; }

string to_string(Clock::time_point tp)
{ return to_string(tp.time_since_epoch()); }

string to_string(Clock::duration d)
{ return std::to_string(d.count()); }




Clock::DurationRange::DurationRange()
: min(0)
, max(32767)
{}

Clock::DurationRange::DurationRange(Clock::rep min, Clock::rep max)
: min(min)
, max(max)
{}


} // namespace gologpp
