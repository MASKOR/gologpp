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


#include "language.h"


namespace gologpp {


class AbstractReference
: public virtual AbstractLanguageElement
{
public:
	AbstractReference();
	virtual ~AbstractReference();

	virtual bool bound() const = 0;
	virtual bool consistent() const = 0;

	virtual const Expression &arg_for_param(shared_ptr<const Variable> param) const = 0;

	void ensure_consistent();
};


}
