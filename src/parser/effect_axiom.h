#ifndef GOLOGPP_PARSER_EFFECT_AXIOM_H_
#define GOLOGPP_PARSER_EFFECT_AXIOM_H_


#include "utilities.h"
#include "reference.h"
#include "formula.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/effect_axiom.h>

namespace gologpp {
namespace parser {


template<class ExprT>
struct EffectParser : grammar<EffectAxiom<ExprT> *(Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect, "effect_axiom")
	{
		effect = ( eps [ _val = new_<EffectAxiom<ExprT>>() ] >> (
			-(condition(_r1) >> "->")
			>> fluent_ref(_r1)
			>> '=' >> expression(_r1)
		)) [
			phoenix::bind(&EffectAxiom<ExprT>::define, *_val, _1, _2, _3)
		];
		effect.name("boolean_effect_axiom");
		on_error<rethrow>(effect, delete_(_val));

		BOOST_SPIRIT_DEBUG_NODE(effect);
	}

	ExpressionParser<BooleanExpression> condition;
	ExpressionParser<ExprT> expression;
	ReferenceParser<Fluent<ExprT>> fluent_ref;
	rule<EffectAxiom<ExprT> *(Scope &)> effect;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EFFECT_AXIOM_H_

