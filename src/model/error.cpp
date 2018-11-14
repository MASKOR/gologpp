#include "error.h"
#include "expressions.h"

namespace gologpp {

Bug::Bug(const std::string &msg)
: msg_(msg)
{}

const char *Bug::what() const noexcept
{ return msg_.c_str(); }



UserError::UserError(const string &msg)
: std::runtime_error(msg)
{}


ExpressionTypeMismatch::ExpressionTypeMismatch(const Expression &expr1, const Expression &expr2)
: UserError("Type of `" + expr1.str() + "' doesn't match type of `" + expr2.str() + "'")
{}



}