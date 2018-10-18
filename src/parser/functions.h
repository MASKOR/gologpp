#ifndef GOLOGPP_PARSER_FUNCTIONS_H_
#define GOLOGPP_PARSER_FUNCTIONS_H_

#include "utilities.h"
#include "statements.h"

namespace gologpp {
namespace parser {


template<class ExpressionT>
struct FunctionParser
: grammar <
	Function<ExpressionT> *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional<vector<shared_ptr<AbstractVariable>>>
	>
> {
	FunctionParser();

	rule<
		Function<ExpressionT> *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<AbstractVariable>>>
		>
	> function;
	StatementParser statement;
};



struct AbstractFunctionParser : grammar<AbstractFunction *(Scope &)> {
	AbstractFunctionParser();

	rule<AbstractFunction *(Scope &)> function;
	FunctionParser<NumericExpression> num_func;
	FunctionParser<BooleanExpression> bool_func;
	FunctionParser<SymbolicExpression> sym_func;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FUNCTIONS_H_

