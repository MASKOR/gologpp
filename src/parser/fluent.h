#ifndef GOLOGPP_PARSER_FLUENT_H_
#define GOLOGPP_PARSER_FLUENT_H_

#include "utilities.h"
#include "atoms.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FluentParser : grammar<Fluent<ExprT> *(Scope &), locals<Scope *>> {
	FluentParser();

	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent;
	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent_forward;
	rule<Fluent<ExprT> *(Scope &), locals<Scope *>> fluent_def;
	rule<InitialValue<ExprT> *> initially;
	decltype(r_name()) fluent_name;
	decltype(abstract_var()) variable;
};

extern FluentParser<BooleanExpression> boolean_fluent;
extern FluentParser<NumericExpression> numeric_fluent;
extern FluentParser<SymbolicExpression> symbolic_fluent;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FLUENT_H_

