#ifndef GOLOGPP_PARSER_EFFECT_AXIOM_H_
#define GOLOGPP_PARSER_EFFECT_AXIOM_H_


#include "utilities.h"
#include "formula.h"


namespace gologpp {
namespace parser {


template<class LhsT>
struct EffectParser : grammar<EffectAxiom<LhsT> *(Scope &), locals<Typename>> {
	EffectParser();

	void init();

	rule<EffectAxiom<LhsT> *(Scope &), locals<Typename>> effect;
	rule<LhsT *(Scope &)> lhs;
	BooleanExpressionParser condition;
};


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EFFECT_AXIOM_H_

