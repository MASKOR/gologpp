#ifndef GOLOGPP_PARSER_EFFECT_AXIOM_H_
#define GOLOGPP_PARSER_EFFECT_AXIOM_H_


#include "utilities.h"
#include "reference.h"
#include "formula.h"
#include "field_access.h"

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


template<class LhsT>
struct EffectParser;


template<class ExprT>
struct EffectParser<Reference<Fluent<ExprT>>> : grammar<EffectAxiom<Reference<Fluent<ExprT>>> *(Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect, "fluent_effect_axiom")
	{
		effect = ( eps [ _val = new_<EffectAxiom<Reference<Fluent<ExprT>>>>() ] >> (
			-(condition(_r1) >> "->")
			>> effect_fluent_ref(_r1)
			>> '=' >> effect_value(_r1)
		)) [
			phoenix::bind(&EffectAxiom<Reference<Fluent<ExprT>>>::define, *_val, _1, _2, _3)
		];
		effect.name("boolean_effect_axiom");
		on_error<rethrow>(effect, delete_(_val));

		BOOST_SPIRIT_DEBUG_NODE(effect);
	}

	ExpressionParser<BooleanExpression> condition;
	ExpressionParser<ExprT> effect_value;
	ReferenceParser<Fluent<ExprT>> effect_fluent_ref;
	rule<EffectAxiom<Reference<Fluent<ExprT>>> *(Scope &)> effect;
};


template<class ExprT>
struct EffectParser<FieldAccess<ExprT>> : grammar<EffectAxiom<FieldAccess<ExprT>> *(Scope &)> {
	EffectParser()
	: EffectParser::base_type(effect, "fluent_field_effect_axiom")
	{
		effect = ( eps [ _val = new_<EffectAxiom<FieldAccess<ExprT>>>() ] >> (
			-(condition(_r1) >> "->")
			>> field_access(_r1)
			>> '=' >> effect_value(_r1)
		)) [
			phoenix::bind(&EffectAxiom<FieldAccess<ExprT>>::define, *_val, _1, _2, _3)
		];
		effect.name("boolean_effect_axiom");
		on_error<rethrow>(effect, delete_(_val));

		BOOST_SPIRIT_DEBUG_NODE(effect);
	}

	ExpressionParser<BooleanExpression> condition;
	ExpressionParser<ExprT> effect_value;
	FieldAccessParser<ExprT> field_access;
	rule<EffectAxiom<FieldAccess<ExprT>> *(Scope &)> effect;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_EFFECT_AXIOM_H_

