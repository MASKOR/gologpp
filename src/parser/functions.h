#ifndef GOLOGPP_PARSER_FUNCTIONS_H_
#define GOLOGPP_PARSER_FUNCTIONS_H_

#include "utilities.h"
#include "statements.h"

namespace gologpp {
namespace parser {


template<class ExpressionT>
struct FunctionParser : grammar<Function<ExpressionT> *(Scope &), locals<Scope *>> {
	FunctionParser();

	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function_forward;
	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function_def;
	rule<Function<ExpressionT> *(Scope &), locals<Scope *>> function;
	StatementParser statement;
};



struct AbstractFunctionParser : grammar<AbstractFunction *(Scope &)> {
	AbstractFunctionParser();

	rule<AbstractFunction *(Scope &)> function;
	FunctionParser<NumericExpression> num_func;
	FunctionParser<BooleanExpression> bool_func;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FUNCTIONS_H_

