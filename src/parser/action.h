#ifndef GOLOGPP_PARSER_ACTION_H_
#define GOLOGPP_PARSER_ACTION_H_

#include "utilities.h"
#include "formula.h"
#include "effect_axiom.h"
#include "domain.h"

#include <model/scope.h>

namespace gologpp {
namespace parser {


struct ActionParser : grammar <
	Action *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < AbstractVariable > > >
	>
> {
	ActionParser();

	ExpressionParser<BooleanExpression> formula_parser;
	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<BooleanExpression> boolean_effect;
	EffectParser<NumericExpression> numeric_effect;
	EffectParser<SymbolicExpression> symbolic_effect;
	AnyDomainAssignmentParser domain_assignment;
	rule < Action *(Scope &), locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < AbstractVariable > > >
	> > action;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ACTION_H_

