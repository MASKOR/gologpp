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

#ifndef GOLOGPP_USER_ERROR_H
#define GOLOGPP_USER_ERROR_H

#include <string>
#include <exception>
#include "gologpp.h"

namespace gologpp {


class SemanticError : public std::exception {
public:
	SemanticError(const std::string &description);
	
	virtual const char *what() const noexcept override;
	
private:
	std::string message_;
};


class InvalidIdentifier : public SemanticError {
public:
	InvalidIdentifier(const std::string &name);
};


class RedefinitionError : public SemanticError {
public:
	RedefinitionError(const string &name);
	RedefinitionError(const string &name, arity_t arity);
};



class NoSuchFieldError : public SemanticError {
public:
	NoSuchFieldError(const string &field_name, const string &type_name);
};


} // namespace gologpp







#endif // GOLOGPP_USER_ERROR_H
