#include "error.h"

namespace gologpp {

Bug::Bug(const std::string &msg)
: msg_(msg)
{}


const char *Bug::what() const noexcept
{ return msg_.c_str(); }



ExpressionTypeMismatch::ExpressionTypeMismatch(const Expression &expr1, const Expression &expr2)
: expr1_(expr1)
, expr2_(expr2)
{}



}