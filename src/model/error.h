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

#ifndef GOLOGPP_ERROR_H_
#define GOLOGPP_ERROR_H_

#include "gologpp.h"

#include <stdexcept>
#include <exception>
#include <string>

namespace gologpp {



class EngineError : public std::runtime_error {
	using std::runtime_error::runtime_error;
};


class DuplicateTransition : public EngineError {
	using EngineError::EngineError;
};

class LostTransition : public EngineError {
	using EngineError::EngineError;
};

class InconsistentTransition : public EngineError {
	using EngineError::EngineError;
};

class Unsupported : public EngineError {
	using EngineError::EngineError;
};



class Bug : public std::exception {
public:
	Bug(const std::string &msg);
	virtual ~Bug() override = default;

	virtual const char *what() const noexcept override;

private:
	std::string msg_;
};



class UserError : public std::runtime_error {
public:
	UserError(const string &msg);
};

class NameNotMapped : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

class SyntaxError : public UserError {
public:
	using UserError::UserError;
	SyntaxError(const string &code, string::size_type offset, const string &msg);
};

class ExpressionTypeMismatch : public UserError {
public:
	ExpressionTypeMismatch(const AbstractLanguageElement &expr1, const AbstractLanguageElement &expr2);
	ExpressionTypeMismatch(const string &msg);
};


class TypeError : public UserError {
public:
	TypeError(const AbstractLanguageElement &expr, const Type &t);
	TypeError(const string &msg);
};


class InvalidIdentifier : public UserError {
public:
	InvalidIdentifier(const std::string &name);
};


class RedefinitionError : public UserError {
public:
	RedefinitionError(const string &name);
	RedefinitionError(const string &name, arity_t arity);
	RedefinitionError(const Global &existing, const string &new_name, const std::type_info &new_type);
};


class NoSuchFieldError : public UserError {
public:
	NoSuchFieldError(const string &field_name, const string &type_name);
};

}



#endif /* GOLOGPP_ERROR_H_ */
