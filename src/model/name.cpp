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

#include "name.h"
#include "logger.h"

namespace gologpp {

string Name::mangle_pfx_ = "gpp~";

Name::Name(const string &name)
: name_(name)
{}

Name::operator string () const
{ return name_; }

bool Name::operator == (const Name &other) const
{ return name() == other.name(); }

bool Name::operator != (const Name &other) const
{ return !(*this == other); }

size_t Name::hash() const
{ return std::hash<string>{}(name()); }

const string &Name::name() const
{ return name_; }

string Name::mangled_name() const
{ return mangle_pfx_ + name_; }

string Name::demangle(const string &s)
{
	if (s.substr(0, mangle_pfx_.length()) != mangle_pfx_) {
		log(LogLevel::ERR) << __func__ << ": name is not mangled: " + s << flush;
		return s;
	}
	else
		return s.substr(mangle_pfx_.length());
}




}
