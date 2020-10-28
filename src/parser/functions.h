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

#ifndef GOLOGPP_PARSER_FUNCTIONS_H_
#define GOLOGPP_PARSER_FUNCTIONS_H_

#include "utilities.h"
#include "statements.h"
#include "mapping.h"

namespace gologpp {
namespace parser {



struct FunctionParser
: grammar <
	Function *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional<vector<shared_ptr<Variable>>>,
		shared_ptr<const Type>
	>
> {
	FunctionParser();

	rule <
		Function *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<Variable>>>,
			shared_ptr<const Type>
		>
	> function;
};



struct ExogFunctionParser
: grammar <
	ExogFunction *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional<vector<shared_ptr<Variable>>>,
		shared_ptr<const Type>
	>
> {
	ExogFunctionParser();

	rule <
		ExogFunction *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<Variable>>>,
			shared_ptr<const Type>
		>
	> function;
};




struct ProcedureParser
: grammar <
	Procedure *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional<vector<shared_ptr<Variable>>>,
		shared_ptr<const Type>
	>
> {
	ProcedureParser();

	rule <
		Procedure *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<Variable>>>,
			shared_ptr<const Type>
		>
	> procedure;

	StatementParser statement;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FUNCTIONS_H_

