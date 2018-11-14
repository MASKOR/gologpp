#ifndef GOLOGPP_PARSER_DOMAIN_H_
#define GOLOGPP_PARSER_DOMAIN_H_

#include "utilities.h"

#include <model/domain.h>

namespace gologpp {
namespace parser {

template<class ExprT>
struct DomainExpressionParser : grammar<Domain<ExprT>(Scope &)> {
	DomainExpressionParser();

	rule<Domain<ExprT>(Scope &)> domain_expr;
	rule<Domain<ExprT>(Scope &)> unary_domain_expr;
	rule<Domain<ExprT>(Scope &)> binary_domain_expr;
	rule<DomainOperator> domain_operator;
};



template<class ExprT>
struct DomainDeclarationParser : grammar<void(Scope &), locals<string>> {
	DomainDeclarationParser();

	rule<void(Scope &), locals<string>> domain;
	DomainExpressionParser<ExprT> domain_expr;
};



template<class ExprT>
struct DomainAssignmentParser : grammar<void(Scope &)> {
	DomainAssignmentParser();

	rule<void(Scope &)> domain_assignment;
	DomainExpressionParser<ExprT> domain_expr;
};



struct AnyDomainAssignmentParser : grammar<void(Scope &)> {
	AnyDomainAssignmentParser();

	rule<void(Scope &)> domain_assignment;
	DomainAssignmentParser<Number> numeric_domain_assignment;
	DomainAssignmentParser<Symbol> symbolic_domain_assignment;
	DomainAssignmentParser<String> string_domain_assignment;
};


} // namespace parser
} // namespace gologpp

#endif //GOLOGPP_PARSER_DOMAIN_H_
