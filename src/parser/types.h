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

#ifndef GOLOGPP_PARSER_TYPES_H_
#define GOLOGPP_PARSER_TYPES_H_

#include <model/gologpp.h>
#include <model/types.h>

#include "utilities.h"


namespace gologpp {
namespace parser {


struct TypeDefinitionParser : public grammar<Type *(Scope &)> {
	TypeDefinitionParser();

	rule<Type *(Scope &)> type_definition;
	rule<CompoundType *(Scope &)> compound_type_def;
	rule<ListType *(Scope &)> list_type_def;
};


struct TypeNameParser : public grammar<Typename(Scope &)> {
	TypeNameParser();

	rule<Typename(Scope &)> list_type_name;
	rule<Typename(Scope &)> type_name;
};

rule<Typename(Scope &)> &any_type_specifier();

template<class BaseT>
rule<shared_ptr<const BaseT>(Scope &)> &complex_type_identifier();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_TYPES_H_
