#include "effect_axiom.h"

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

#include "reference.h"
#include "field_access.h"
#include "expressions.h"


namespace gologpp {
namespace parser {


template<class LhsT>
EffectParser<LhsT>::EffectParser()
: EffectParser<LhsT>::base_type(effect, "effect_axiom")
{
	effect = ( eps [ _val = new_<EffectAxiom<LhsT>>() ] >> (
		-(lit("if") > '(' > condition(_r1) > ')')
		>> lhs(_r1)
		>> '=' >> typed_expression(_r1, phoenix::bind(&Expression::type_name, _1))
	)) [
		phoenix::bind(&EffectAxiom<LhsT>::define, *_val, _1, _2, _3)
	];
	effect.name("effect_axiom");
	on_error<rethrow>(effect, delete_(_val));

	GOLOGPP_DEBUG_NODE(effect);
}



}
}
