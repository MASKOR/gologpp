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

#include "gologpp.h"

namespace gologpp {


class Name {
public:
    Name(const string &name);
    virtual ~Name() = default;

    operator string () const;
    virtual bool operator == (const Name &other) const;
    bool operator != (const Name &other) const;
    virtual size_t hash() const;
    const string &name() const;
	string mangled_name() const;
	static string demangle(const string &s);

private:
	static const string mangle_pfx_;

protected:
    string name_;
};


}
