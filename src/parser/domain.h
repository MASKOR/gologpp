#ifndef GOLOGPP_PARSER_DOMAIN_H_
#define GOLOGPP_PARSER_DOMAIN_H_

#include "utilities.h"
#include "atoms.h"

#include <model/domain.h>

namespace gologpp {
namespace parser {

struct DomainExpressionParser : grammar<Domain(Scope &, Typename, bool)> {
	DomainExpressionParser();

	rule<Domain(Scope &, Typename, bool)> domain_expr;
	rule <
		Domain(Scope &, Typename, bool),
		locals<shared_ptr<Domain>>
	> unary_domain_expr;
	rule<Domain(Scope &, Typename, bool)> binary_domain_expr;
	rule<DomainOperator()> domain_operator;
};

rule<Domain(Scope &, Typename)> &domain_expression();
rule<void(Scope &), locals<string, Typename>> &domain_decl();
rule<void(Scope &, bool), locals<Typename>> &domain_assignment();


} // namespace parser
} // namespace gologpp

#endif //GOLOGPP_PARSER_DOMAIN_H_
