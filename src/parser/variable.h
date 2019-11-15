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

#ifndef GOLOGPP_PARSER_VARIABLE_H_
#define GOLOGPP_PARSER_VARIABLE_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl();
rule<shared_ptr<Variable>(Scope &, const Type &)> &var_usage();
rule<Reference<Variable> *(Scope &, const Type &)> &var_ref();
rule<shared_ptr<Variable>(Scope &)> &any_var_usage();


rule<Expression *()> &atom();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_VARIABLE_H_

