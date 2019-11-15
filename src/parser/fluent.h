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

#ifndef GOLOGPP_PARSER_FLUENT_H_
#define GOLOGPP_PARSER_FLUENT_H_

#include "utilities.h"
#include "domain.h"

namespace gologpp {
namespace parser {


struct FluentParser
: grammar <
	Fluent *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < Variable > > >,
		PType
	>
> {
	FluentParser();

	rule <
		Fluent *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional < vector < shared_ptr < Variable > > >,
			PType
		>
	> fluent;

	rule<Expression *(Scope &)> initial_val_arg;
	rule<InitialValue *(Scope &, const Type &)> initially;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FLUENT_H_

