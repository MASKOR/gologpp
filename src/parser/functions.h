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
		boost::optional<vector<shared_ptr<AbstractVariable>>>,
		string
	>
> {
	FunctionParser();

	rule<
		Function<ExpressionT> *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional<vector<shared_ptr<AbstractVariable>>>,
			string
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
	FunctionParser<StringExpression> string_func;
	FunctionParser<CompoundExpression> compound_func;
	FunctionParser<VoidExpression> procedure;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FUNCTIONS_H_

