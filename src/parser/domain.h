/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef GOLOGPP_PARSER_DOMAIN_H_
#define GOLOGPP_PARSER_DOMAIN_H_

#include "utilities.h"

#include <model/domain.h>

namespace gologpp {
namespace parser {

struct DomainExpressionParser : grammar<Domain(Scope &, const Type &, bool)> {
	DomainExpressionParser();

	rule<Domain(Scope &, const Type &, bool)> domain_expr;
	rule <
		Domain(Scope &, const Type &, bool),
		locals<shared_ptr<Domain>>
	> unary_domain_expr;
	rule<Domain(Scope &, const Type &, bool)> binary_domain_expr;
	rule<DomainOperator()> domain_operator;
};

rule<Domain(Scope &, const Type &)> &domain_expression();
rule<void(Scope &), locals<string, shared_ptr<const Type>>> &domain_decl();


} // namespace parser
} // namespace gologpp

#endif //GOLOGPP_PARSER_DOMAIN_H_
