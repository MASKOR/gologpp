#ifndef GOLOGPP_PARSER_FLUENT_H_
#define GOLOGPP_PARSER_FLUENT_H_

#include "utilities.h"
#include "atoms.h"
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
		Typename
	>
> {
	FluentParser();

	rule <
		Fluent *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional < vector < shared_ptr < Variable > > >,
			Typename
		>
	> fluent;

	rule<Expression *(Scope &)> initial_val_arg;
	rule<InitialValue *(Scope &, Typename)> initially;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FLUENT_H_

