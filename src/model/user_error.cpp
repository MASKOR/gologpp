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
