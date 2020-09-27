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


rule<Value *()> &numeric_value();
rule<Value()> &numeric_value_o();
rule<Value *()> &boolean_value();
rule<Value *()> &string_value();
rule<Value *()> &symbolic_value();
rule<Value *()> &symbolic_value_def();
rule<Value *()> &compound_value();
rule<Value *()> &list_value();
rule<Value *()> &undefined_value();
rule<Value()> &undefined_value_o();

rule<Value *()> &any_value();

rule<Value *(const Type &, bool /*allow_symbol_def*/)> &value();


void initialize_cyclic_values();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_VALUE_H_

