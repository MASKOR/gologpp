#ifndef GOLOGPP_PARSER_FUNCTIONS_H_
#define GOLOGPP_PARSER_FUNCTIONS_H_

#include "utilities.h"

namespace gologpp {
namespace parser {


struct FunctionParser
: grammar <
	Function *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional<vector<shared_ptr<Variable>>>,
		string
	>
> {
	FunctionParser();

	rule<
		Function *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<Variable>>>,
			string
		>
	> function;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FUNCTIONS_H_

