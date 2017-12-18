#include "user_error.h"


namespace gologpp {

using namespace std;

SemanticError::SemanticError(const string &description)
: message_(description)
{}


const char *SemanticError::what() const noexcept
{ return message_.c_str(); }


} // namespace gologpp
