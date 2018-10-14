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



} // namespace gologpp
