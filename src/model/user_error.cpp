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



RedeclarationError::RedeclarationError(const string &name)
: SemanticError("Symbol already declared: " + name)
{}

RedeclarationError::RedeclarationError(const string &name, arity_t arity)
: SemanticError("Symbol already declared: " + name + "/" + std::to_string(arity))
{}



} // namespace gologpp
