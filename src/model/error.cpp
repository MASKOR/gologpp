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

#include "language.h"
#include "types.h"
#include "error.h"
#include "global.h"

#include <boost/core/demangle.hpp>

#include <parser/utilities.h>

namespace gologpp {


Bug::Bug(const std::string &msg)
: msg_(msg)
{}

const char *Bug::what() const noexcept
{ return msg_.c_str(); }



UserError::UserError(const string &msg)
: std::runtime_error(msg)
{}


SyntaxError::SyntaxError(const string &code, string::size_type offset, const string &msg)
: UserError(
	parser::get_error_context(
		parser::iterator(code.begin()),
		parser::iterator(code.begin() + string::difference_type(offset)),
		parser::iterator(code.end())
	) + "\n" + msg
)
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
: UserError("Expression " + expr.str() + " is not of type " + t.name())
{}


TypeError::TypeError(const string &msg)
: UserError(msg)
{}



InvalidIdentifier::InvalidIdentifier(const std::string &name)
: UserError("Invalid identifier: " + name)
{}



RedefinitionError::RedefinitionError(const string &name)
: UserError("Symbol already defined: " + name)
{}

RedefinitionError::RedefinitionError(const string &name, arity_t arity)
: UserError("Symbol already defined: " + name + "/" + std::to_string(arity))
{}

RedefinitionError::RedefinitionError(const Global &existing, const string &new_name, const std::type_info &new_type)
: UserError(
	"Cannot define or declare \"" + new_name + "\" as a " + boost::core::demangle(new_type.name())
	+ " because it is already defined as a " + boost::core::demangle(typeid(existing).name())
  )
{}




NoSuchFieldError::NoSuchFieldError(const string &field_name, const string &type_name)
: UserError("No field `" + field_name + "\' in type `" + type_name + "\'")
{}


}
