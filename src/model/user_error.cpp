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

#include "user_error.h"


namespace gologpp {

SemanticError::SemanticError(const std::string &description)
: message_(description)
{}

const char *SemanticError::what() const noexcept
{ return message_.c_str(); }



InvalidIdentifier::InvalidIdentifier(const std::string &name)
: SemanticError("Invalid identifier: " + name)
{}



RedefinitionError::RedefinitionError(const string &name)
: SemanticError("Symbol already defined: " + name)
{}

RedefinitionError::RedefinitionError(const string &name, arity_t arity)
: SemanticError("Symbol already defined: " + name + "/" + std::to_string(arity))
{}



NoSuchFieldError::NoSuchFieldError(const string &field_name, const string &type_name)
: SemanticError("No field `" + field_name + "\' in type `" + type_name + "\'")
{}



} // namespace gologpp
