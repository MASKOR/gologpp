#ifndef GOLOGPP_PARSER_DOMAIN_H_
#define GOLOGPP_PARSER_DOMAIN_H_

#include "utilities.h"
#include "atoms.h"

#include <model/domain.h>

namespace gologpp {
namespace parser {

struct DomainExpressionParser : grammar<Domain(Scope &, Typename)> {
	DomainExpressionParser();

	rule<Domain(Scope &, Typename)> domain_expr;
	rule<Domain(Scope &, Typename)> unary_domain_expr;
	rule<Domain(Scope &, Typename)> binary_domain_expr;
	rule<DomainOperator()> domain_operator;
};

extern rule<Domain(Scope &, Typename)> domain_expression;
extern rule<void(Scope &), locals<string, Typename>> domain_decl;
extern rule<void(Scope &), locals<Typename>> domain_assignment;


} // namespace parser
} // namespace gologpp

#endif //GOLOGPP_PARSER_DOMAIN_H_
