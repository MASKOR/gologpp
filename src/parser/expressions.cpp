#include "expressions.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/self.hpp>

#include "arithmetic.h"
#include "formula.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"

namespace gologpp {
namespace parser {


rule<Expression *(Scope &)> value_expression {
	numeric_expression(_r1)
	| boolean_expression(_r1)
	| string_expression(_r1)
	| symbolic_expression(_r1)
	| compound_expression(_r1)
	, "value_expression"
};


rule<Expression *(Scope &, Typename)> typed_expression {
	value_expression(_r1) [
		_pass = phoenix::bind(&Expression::type_name, _1) == _r2
	]
};

}
}
