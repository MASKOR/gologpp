#include "error.h"

namespace gologpp {


Bug::Bug(const std::string &msg)
: msg_(msg)
{}

const char *Bug::what() const noexcept
{ return msg_.c_str(); }



UserError::UserError(const string &msg)
: std::runtime_error(msg)
{}


ExpressionTypeMismatch::ExpressionTypeMismatch(const AbstractLanguageElement &expr1, const AbstractLanguageElement &expr2)
: UserError(
	"Type `" + expr1.type().name()
	+ "'of `" + expr1.str()
	+ "' doesn't match type `" + expr2.type().name()
	+ "' of `" + expr2.str() + "'"
)
{}


ExpressionTypeMismatch::ExpressionTypeMismatch(const string &msg)
: UserError(msg)
{}


TypeError::TypeError(const AbstractLanguageElement &expr, const Type &t)
: UserError("Expression `" + expr.str() + "` is not of type " + t.name())
{}


TypeError::TypeError(const string &msg)
: UserError(msg)
{}


}
