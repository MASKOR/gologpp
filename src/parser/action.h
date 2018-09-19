#ifndef GOLOGPP_PARSER_ACTION_H_
#define GOLOGPP_PARSER_ACTION_H_

#include "utilities.h"
#include "formula.h"
#include "effect_axiom.h"

#include <model/scope.h>

namespace gologpp {
namespace parser {


struct ActionParser : grammar<Action *(Scope &)> {
	ActionParser();

	BooleanExpressionParser formula_parser;
	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<BooleanExpression> boolean_effect;
	EffectParser<NumericExpression> numeric_effect;
	rule<Action *(Scope &)> action;
	rule<Action *(Scope &), locals<Scope *>> action_forward;
	rule<Action *(Scope &), locals<Scope *>> action_def;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ACTION_H_

