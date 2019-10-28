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

#ifndef GOLOGPP_PARSER_VALUE_H_
#define GOLOGPP_PARSER_VALUE_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<Value *()> &numeric_literal();
rule<Value *()> &boolean_literal();
rule<Value *()> &string_literal();
rule<Value *()> &symbolic_literal();
rule<Value *()> &symbolic_literal_def();
rule<Value *()> &compound_literal();
rule<Value *()> &list_literal();
rule<Value *()> &undefined_literal();

rule<Value *()> &any_literal();

rule<Value *(Typename, bool)> &literal();


void initialize_cyclic_literals();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_VALUE_H_

