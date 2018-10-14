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


class RedeclarationError : public SemanticError {
public:
	RedeclarationError(const string &name);
	RedeclarationError(const string &name, arity_t arity);
};



} // namespace gologpp







#endif // GOLOGPP_USER_ERROR_H
