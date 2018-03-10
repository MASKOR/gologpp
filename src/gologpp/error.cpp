#include "error.h"

namespace gologpp {

Bug::Bug(const string &msg)
: msg_(msg)
{}


const char *Bug::what() const noexcept
{ return msg_.c_str(); }


}