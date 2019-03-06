#include "reference.h"
#include "field_access.h"
#include "types.h"
#include "compound_expression.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {


template<class ExprT>
FieldAccessParser<ExprT>::FieldAccessParser()
: FieldAccessParser<ExprT>::base_type(field_access, type_descr<ExprT>() + "_field_access")
{
	field_access = ((compound_expression(_r1) >> lit('.')) > type_mark<ExprT>() > r_name()) [
		_val = new_<FieldAccess<ExprT>>(_1, _2)
	];
}

#define GOLOGPP_PARSER_INSTANTIATE_FIELD_ACCESS_PARSER(_r, _data, T) \
	template \
	FieldAccessParser<T>::FieldAccessParser();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_INSTANTIATE_FIELD_ACCESS_PARSER, (), GOLOGPP_VALUE_TYPES)


} // namespace parser
} // namespace gologpp
