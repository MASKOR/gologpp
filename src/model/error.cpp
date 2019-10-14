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

#include "error.h"

namespace gologpp {


Bug::Bug(const std::string &msg)
: msg_(msg)
{}

const char *Bug::what() const noexcept
{ return msg_.c_str(); }



UserError::UserError(const string &msg)
: std::runtime_error(msg)
{}


ExpressionTypeMismatch::ExpressionTypeMismatch(const AbstractLanguageElement &expr1, const AbstractLanguageElement &expr2)
: UserError(
	"Type `" + expr1.type().name()
	+ " 'of `" + expr1.str()
	+ "' doesn't match type `" + expr2.type().name()
	+ "' of `" + expr2.str() + "'"
)
{}


ExpressionTypeMismatch::ExpressionTypeMismatch(const string &msg)
: UserError(msg)
{}


TypeError::TypeError(const AbstractLanguageElement &expr, const Type &t)
: UserError("Expression `" + expr.str() + "` is not of type " + t.name())
{}


TypeError::TypeError(const string &msg)
: UserError(msg)
{}


}
