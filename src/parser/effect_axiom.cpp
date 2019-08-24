#include "effect_axiom.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_lit.hpp>

#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/fusion/at.hpp>

#include <model/effect_axiom.h>

#include "reference.h"
#include "field_access.h"
#include "list_access.h"
#include "expressions.h"


namespace gologpp {
namespace parser {


template<class LhsT>
EffectParser<LhsT>::EffectParser()
: EffectParser<LhsT>::base_type(effect, "effect_axiom")
{
	effect = (
		(-(lit("if") > '(' > condition(_r1) > ')')
		>> lhs(_r1) [
			_a = phoenix::bind(&Expression::type_name, *_1)
		])
		> '=' > typed_expression()(_r1, _a)
	) [
		_val = new_<EffectAxiom<LhsT>>(),
		phoenix::bind(&EffectAxiom<LhsT>::define, *_val, at_c<0>(_1), at_c<1>(_1), _2)
	];
	effect.name("effect_axiom");
	on_error<rethrow>(effect, delete_(_val));

	init();

	GOLOGPP_DEBUG_NODES((effect)(lhs))
}


template<>
void EffectParser<Reference<Fluent>>::init()
{
	static ReferenceParser<Fluent> ref;
	lhs = ref(_r1);
}


template<>
void EffectParser<FieldAccess>::init()
{
	lhs = mixed_field_access()(_r1, UndefinedType::name());
}


template<>
void EffectParser<ListAccess>::init()
{
	lhs = mixed_list_access()(_r1, UndefinedType::name());
}

template
struct EffectParser<Reference<Fluent>>;

template
struct EffectParser<FieldAccess>;


}
}
