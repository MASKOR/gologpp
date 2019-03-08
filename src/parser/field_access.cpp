#include "reference.h"
#include "field_access.h"
#include "types.h"
#include "compound_expression.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {


template<class ExprT>
FieldAccessParser<ExprT>::FieldAccessParser()
: FieldAccessParser<ExprT>::base_type(field_access, type_descr<ExprT>() + "_field_access")
{
	// TODO: Nested field access!
	field_access = ((unary_compound_expr(_r1) >> lit('.')) > type_mark<ExprT>() > r_name()) [
		_val = new_<FieldAccess<ExprT>>(_1, _2)
	];
	field_access.name(type_descr<ExprT>() + "_field_access");

	unary_compound_expr = compound_constant
		| var<CompoundExpression>()(_r1) [ _val = new_<Reference<CompoundVariable>>(_1) ]
		| compound_fluent_ref(_r1) | compound_function_ref(_r1);

	BOOST_SPIRIT_DEBUG_NODES((field_access)(unary_compound_expr));
}

#define GOLOGPP_PARSER_INSTANTIATE_FIELD_ACCESS_PARSER(_r, _data, T) \
	template \
	FieldAccessParser<T>::FieldAccessParser();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_INSTANTIATE_FIELD_ACCESS_PARSER, (), GOLOGPP_VALUE_TYPES)


} // namespace parser
} // namespace gologpp
