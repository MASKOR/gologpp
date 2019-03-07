#ifndef GOLOGPP_PARSER_FLUENT_H_
#define GOLOGPP_PARSER_FLUENT_H_

#include "utilities.h"
#include "atoms.h"
#include "domain.h"

namespace gologpp {
namespace parser {


template<class ExprT>
struct FluentParser
: grammar <
	Fluent<ExprT> *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < AbstractVariable > > >,
		string
	>
> {
	FluentParser();

	rule <
		Fluent < ExprT > *(Scope &),
		locals <
			Scope *,
			string,
			boost::optional < vector < shared_ptr < AbstractVariable > > >,
			string
		>
	> fluent;
	rule<InitialValue<ExprT> *()> initially;
	decltype(r_name()) fluent_name;
	decltype(abstract_var<true>()) variable;
	AnyDomainAssignmentParser domain_assignment;
	ConstantParser<ExprT, true> constant;
};

extern FluentParser<NumericExpression> numeric_fluent;
extern FluentParser<BooleanExpression> boolean_fluent;
extern FluentParser<SymbolicExpression> symbolic_fluent;
extern FluentParser<StringExpression> string_fluent;
extern FluentParser<CompoundExpression> compound_fluent;


rule<AbstractFluent *(Scope &)> &abstract_fluent();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_FLUENT_H_

